//DUMB server

#include"DUMB.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>
#include<signal.h>

struct messageBox* boxList = NULL;

//server will maintain a list of all the clients that are currently connected
struct threadNode* clientList = NULL;

//add a client to the list of clients
void addClient(struct thread* client){
	struct threadNode* temp = clientList;
	
	//first client
	if(temp == NULL){
		temp = (struct threadNode*)malloc(sizeof(struct threadNode));
		temp->client = client;
		temp->next = NULL;
		clientList = temp;
	}else{
		//append client to the end of the list of clients
		while(temp->next != NULL){
			temp = temp->next;
		}
		struct threadNode* newClient = (struct threadNode*)malloc(sizeof(struct threadNode));
		newClient->client = client;
		newClient->next = NULL;
		temp->next = newClient;
	}
}

//catches sig int, then closes all threads
void sigHandler(int sig){
	printf("\nSignal %d was caught. Closing server now.\n", sig);
	exit(0);
}

int main(int argc, char** argv){
	if(argc!=2){
		printf("Error: Please enter a port number.\n");
		return 0;
	}
	
	int port = atoi(argv[1]);
	int sd;
	
	//create the socket
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Error: Socket creation failed.\n");
		return 0;
	}
	
	struct sockaddr_in server_addr; //set up server
	int addrlen = sizeof(server_addr); //length of address
	
	memset(&server_addr, '0', sizeof(server_addr));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	
	//bind using the socket descriptor
	int b;
	if((b = bind(sd, (struct sockaddr*)&server_addr, addrlen)) == -1){
		printf("Error: Bind failed.\n");
		return 0;
	}
	
	//listen to clients with a max number of client connections
	int maxConnections = 10;
	int list;
	if((list = listen(sd, maxConnections)) == -1){
		printf("Error: Listen failed.\n");
		return 0;
	}
	
	struct sockaddr_in client_addr;
	int cd; //client descriptor
	int clilen = sizeof(client_addr);
	
	//The server must keep running & accepting clients until cntrl+C is received to end it in the cmd line
	signal(SIGINT, sigHandler);
	while(1){
		//
		//accept client
		//printf("Waiting for clients...\n");
		if((cd = accept(sd, (struct sockaddr*)&client_addr, &clilen)) == -1){
			printf("Error: Accept failed.\n");
			return 0;
		}
		
		//printf("Client found.\n");
		
		//create thread for each unique client
		struct thread* ct = (struct thread*)malloc(sizeof(struct thread)); //client thread structure to hold info about ea. thread
		pthread_t* clientThread = malloc(sizeof(pthread_t)); //malloc the thread
		ct->th = clientThread;
		ct->sd = cd;
		
		int pthread; //catch errors (to respond to client errors? not sure yet)
		if((pthread = pthread_create(clientThread, NULL, receiveCommands, (void*)ct)) == -1){
			printf("Error: %d Message%s\n", errno, strerror(errno));
		}
		
		addClient(ct);
	}
	
	return 0;
}

void CREAT(char* name){
	struct messageBox* newBox = (struct messageBox*)malloc(sizeof(struct messageBox));
	newBox->boxName = name;
	newBox->next = NULL;
	newBox->open = 0; //before we implement mutexes
	newBox->empty = 0; //initially has no msgs
	if(boxList == NULL){
		boxList = newBox;
	}else{
		struct messageBox* temp = boxList;
		
		while(temp->next!=NULL){
			temp = temp->next;
		}
		
		temp->next = newBox;
	}
	
	return;
}

struct messageBox* getBX(char* name){
  struct messageBox *ptr = (struct messageBox*)malloc(sizeof(struct messageBox));
  while(ptr != NULL){
    if(strcmp(ptr->boxName, name) == 0){
      return ptr;
    }
    ptr = ptr->next;
  }
  //Should never get to here
  return NULL;
}


int OPNBX(char* name){
  struct messageBox *ptr = (struct messageBox*)malloc(sizeof(struct messageBox));
  while(ptr != NULL){
    if(strcmp(ptr->boxName, name) == 0 && ptr->open == 0){
      //found the box and it isn't currently open
      return 1;
    }
    ptr = ptr->next;
  }
  //the box is open
  return 0;
}

int NXTMG();

int DELBX(char* name){
	struct messageBox* temp = boxList;
	struct messageBox* prev = NULL;
	while(temp->next!=NULL){
		if(temp->boxName == name){
			if(temp->empty == 0){ //box is not empty
				return -1;
			}else if(temp->open == 1){ //box is currently opened by another user
				return -2;
			}else{
				break;
			}
		}else{
			prev = temp;
			temp = temp->next;
		}
	}
	
	prev->next = temp->next;
	free(temp);
	return 1;
}
//gets message from queue and sends to client
void pop(int sd, struct messageBox* ptr){
  struct message* temp = ptr->mymsg;
  char* retval = "OK!";
  int len = strlen(temp->msg);
  char* length;
  sprintf(length, "%d", len);
  strcat(retval, length);
  strcat(retval, "!");
  strcat(retval, temp->msg);
  sendMessage(sd, retval);
  ptr->mymsg = temp->next;
  free(temp);
  return;
}

void* receiveCommands(void* args){
	struct thread* client = (struct thread*)args;
	int sd = client->sd;
	char* action = (char*)malloc(sizeof(char)*100);
	char* command = (char*)malloc(sizeof(char)*10);
	char* msg;
	int valread, i;
	
	readMessage(sd, action);
	
	//E0: HELLO
	if(strcmp(action,"HELLO") == 0){
		msg = "HELLO DUMBv0 ready!";
		sendMessage(sd, msg);
	}else{
		printf("Error: HELLO was not detected. Closing connection.\n");
		pthread_exit(NULL);
	}
	
	int status;
	struct messageBox* current = (struct messageBox*)malloc(sizeof(struct messageBox));
	while(1){
		readMessage(sd, action);
		
		status = 0;
		char command[6];
		strncpy(command, action, 5);
		command[5] = '\0';
	
		if(strcmp(command, "GDBYE") == 0){
			break;
		}else if(strcmp(command, "CREAT") == 0){
			char* boxName = &action[6];
			if(checkExistingBoxName(boxName, boxList) == 1){
				CREAT(boxName);
				sendMessage(sd, "OK!");
			}else{
				sendMessage(sd, "ER:EXIST");
			}
		}else if(strcmp(command, "OPNBX") == 0){
			char* boxName = &action[6];
			status = checkExistingBoxName(boxName, boxList);
			if(status == 1){
				status = OPNBX(boxName);
				if(status == 1){
					sendMessage(sd, "OK!");
					current = getBX(boxName);
				}else{
					sendMessage(sd, "ER:OPEND");
				}
			}else{
				sendMessage(sd, "ER:NEXST");
			}
		}else if(strcmp(command, "NXTMG") == 0){
		  if(current == NULL){
		    sendMessage(sd, "ER:NOOPN");
		  }else if(current->mymsg == NULL){
		    sendMessage(sd, "ER:EMPTY");
		  }else{
		    pop(sd, current);
		  }
		}else if(strcmp(command, "PUTMG") == 0){
		  
		}else if(strcmp(command, "DELBX") == 0){
			char* boxName = &action[6];
			status = checkExistingBoxName(boxName, boxList);
			if(status == 1){
				status = DELBX(boxName);
				if(status == 1){
					sendMessage(sd, "OK!");
				}else if(status == -1){
					sendMessage(sd, "ER:NOTMT");
				}else{
					sendMessage(sd, "ER:OPEND");
				}
			}else{
				sendMessage(sd, "ER:NEXST\n");
			}
		}else if(strcmp(command, "CLSBX") == 0){
	
		}else{
			sendMessage(sd, "ER:WHAT?\n");
		}
	}
	
	close(sd);
	pthread_exit(NULL);
}

