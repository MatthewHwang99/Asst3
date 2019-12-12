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
	//if(sig == 2){
		printf("\nSignal %d was caught. Closing server now.\n", sig);
		struct threadNode* cliLL = clientList;
		if(cliLL == NULL){
			exit(0);
		}else{
			while(cliLL!=NULL){
			struct thread* cli = cliLL->client;
			pthread_t* thc = cli->th;
			pthread_join(*thc, NULL);
			cliLL = cliLL->next;
			}
			exit(0);
		}
	//}
	
	exit(0);
}

void printBoxes(){
	struct messageBox* temp = boxList;
	if(temp == NULL){
		printf("Empty.\n");
	}else{
		int i = 1;
		while(temp != NULL){
			printf("Box(%d): %s --> ", i, temp->boxName);
			temp = temp->next;
			i++;
		}
	}
	
	return;
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
	
	while(1){
		signal(SIGPIPE, SIG_IGN);
		signal(SIGINT, sigHandler);
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
	newBox->mymsg = NULL;
	pthread_mutex_t lock;
	pthread_mutex_init(&lock, NULL);
	newBox->box_lock = lock;
	
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
  struct messageBox *ptr = boxList;
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
	struct messageBox *ptr = boxList;
	while(ptr != NULL){
    	if(strcmp(ptr->boxName, name) == 0){
			break;
    	}else{
    		ptr = ptr->next;
    	}
 	}
 	
 	if(pthread_mutex_trylock(&ptr->box_lock) == 0){
 		//box was unlocked, give the lock to the client
 		//pthread_mutex_lock(&ptr->box_lock);
 		return 1;
 	}else{
 		return -1;
 	}
 	
 	
}

int PUTMG(char* message, struct messageBox** box){
  struct message *ptr = (struct message*)malloc(sizeof(message));
  struct message *new = (struct message*)malloc(sizeof(message));
  new->msg = message;
  new->next = NULL;
  ptr = (*box)->mymsg;
  while(ptr->next != NULL){
    ptr = ptr->next;
  }
  ptr->next = new;
  return 1;
}

/*
int PUTMG(char* name, char* msg){
	struct messageBox* temp = boxList;
	
	while(temp!=NULL){
		if(temp->boxName == name){
			break;
		}else{
			temp = temp->next;
		}
	}
	
	struct message* msgs = temp->mymsg;
	struct message* newmsg = (struct message*)malloc(sizeof(struct message));
	newmsg->msg = msg;
	newmsg->next = NULL;
	
	if(msgs == NULL){ //currently no messages in the message box, this will be the first message
		msgs = newmsg;
	}else{
		struct message* prevmsg = NULL;
		while(msgs!=NULL){ //go to the last msg in the list AKA queue
			prevmsg = msgs;
			msgs = msgs->next;
		}
		
		prevmsg->next = newmsg;
	}
	
	return 1;
}
*/
int DELBX(char* name){
	struct messageBox* temp = boxList;
	struct messageBox* prev = NULL;
	while(temp->next!=NULL){
		if(temp->boxName == name){
			break;
		}else{
			prev = temp;
			temp = temp->next;
		}
	}
	
	if(pthread_mutex_trylock(&temp->box_lock) == 0){
		pthread_mutex_unlock(&temp->box_lock);
		if(temp->mymsg != NULL){ //box is not empty, can't be deleted yet
			return -2;
		}
		if(prev == NULL){
			//the only messageBox was deleted
			boxList = NULL;
			free(temp);
			return 1;
		}else{
			prev->next = temp->next;
			free(temp);
			return 1;
		}
	}else{
		return -1; //box is already open/locked
	}
}

int CLSBX(char* name){
	struct messageBox *ptr = boxList;
	while(ptr != NULL){
    	if(strcmp(ptr->boxName, name) == 0){
			break;
    	}else{
    		ptr = ptr->next;
    	}
 	}
 	
 	if(pthread_mutex_unlock(&ptr->box_lock) == 0){
 		//box was locked by the client
 		return 1;
 	}else{
 		return -1;
 	}
}

void* receiveCommands(void* args){
	struct thread* client = (struct thread*)args;
	int sd = client->sd;
	char* action = (char*)malloc(sizeof(char)*100);
	char* command = (char*)malloc(sizeof(char)*10);
	char* msg;
	int valread, i;
	int hasBox = 0;
	
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
	struct messageBox* current = NULL;
	pthread_mutex_t cli_lock;
	
	while(1){
		memset(action, 0, 100);
		readMessage(sd, action);
		//printBoxes();
		status = 0;
		char* command = malloc(10);
		strncpy(command, action, 5);
		command[5] = '\0';
		
		//printf("Received: %s\n", action);
	
		if(strcmp(command, "GDBYE") == 0){
			break;
		}else if(strcmp(command, "CREAT") == 0){
			char* boxName = malloc(strlen(action));
			strcpy(boxName, &action[6]);
			if(checkExistingBoxName(boxName, boxList) == -1){
				CREAT(boxName);
				sendMessage(sd, "OK!");
			}else{
				sendMessage(sd, "ER:EXIST");
			}
		}else if(strcmp(command, "OPNBX") == 0){
			char* boxName = malloc(strlen(action));
			strcpy(boxName, &action[6]);
			status = checkExistingBoxName(boxName, boxList);
			
			//Occurs if the box already has a box open, therefore it cannot open another box
			if(current!=NULL){
				sendMessage(sd, "ER:BXOPN");
			}
			
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
		 	if(current == NULL){
		  	 	sendMessage(sd, "ER:NOOPN");
		  	}else{
			  //parse the string here: PUTMG!12!Oh hai, Mark!
			  char *input = (char*)malloc(strlen(action));
			  input = action;
			  char* p;
			  int size;
			  p = strtok(input, "!");
			  p = strtok(NULL, "!");
			  size = atoi(p);
			  p = strtok(NULL, "!");
			  PUTMG(p, &current);
			  char* returnvalue = (char*)malloc(strlen(p));
			  returnvalue = "OK!";
			  strncat(returnvalue, p, strlen(p));
			  sendMessage(sd, returnvalue);
		 	}
		}else if(strcmp(command, "DELBX") == 0){
			char* boxName = malloc(strlen(action));
			strcpy(boxName, &action[6]);
			status = checkExistingBoxName(boxName, boxList);
			if(status == 1){
				status = DELBX(boxName);
				if(status == 1){ //box successfully deleted
					sendMessage(sd, "OK!");
				}else if(status == -1){  //box is already opened/locked
					sendMessage(sd, "ER:OPEND");
				}else{ //box is not empty
					sendMessage(sd, "ER:NOTMT");
				}
			}else{
				sendMessage(sd, "ER:NEXST\n");
			}
		}else if(strcmp(command, "CLSBX") == 0){
			char* boxName = malloc(strlen(action));
			strcpy(boxName, &action[6]);
			status = checkExistingBoxName(boxName, boxList);
			if(status == 1){
				status = CLSBX(boxName);
				sendMessage(sd, "OK!");
				current = NULL;
			}else{
				sendMessage(sd, "ER:NOOPN");
			}
		}else{
			sendMessage(sd, "ER:WHAT?\n");
		}
	}
	
	close(sd);
	pthread_exit(NULL);
}

