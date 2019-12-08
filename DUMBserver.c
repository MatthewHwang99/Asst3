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

struct thread{
	pthread_t* th;
	int sd;
	int retval;
};

struct threadNode{
	struct thread* client;
	struct threadNode* next;
};

struct message{
	char* msg;
	struct message* next;
};

struct messageBox{
	struct message* mymsg;
	struct messageBox* next;
};

void* receiveCommands(void*);
void sigHandler(int);
void addClient(struct thread*);

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
	
	while(1){
		readMessage(sd, action);
		
		if(strcmp(command,"quit") == 0){ //E.1 GDBYE
		
		}else if(strcmp(command,"create") == 0){ //E.2 CREAT arg0
	
		}else if(strcmp(command,"open") == 0){ //E.3 OPNBX arg0
	
		}else if(strcmp(command,"next") == 0){ //E.4 NXTMG
	
		}else if(strcmp(command,"put") == 0){ //E.5 PUTMG!arg0!msg
	
		}else if(strcmp(command,"delete") == 0){ //E.6 DELBX arg0
	
		}else if(strcmp(command,"close") == 0){ //E.7 CLSBX arg0
	
		}else{
			printf("Error: Command not recognized.\n");
			pthread_exit(NULL);
		}
	}
	
	close(sd);
	pthread_exit(NULL);
}

