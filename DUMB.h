#ifndef _DUMB_h
#define _DUMB_h

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<netdb.h>
#include<unistd.h>
#include<errno.h>
#include<dirent.h>
#include<fcntl.h>
#include<openssl/sha.h>
#include<netinet/in.h>
#include<math.h>
#include<pthread.h>
#include<arpa/inet.h>

//structs used by server
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
	char* boxName;
	struct message* mymsg;
	struct messageBox* next;
	pthread_mutex_t box_lock;
};

//functions used by server
void* receiveCommands(void*);
void sigHandler(int);
void addClient(struct thread*);
int checkExistingBoxName(char*, struct messageBox*);
void pop(int, struct messageBox*);
void printBoxes();

//functions used by client
void help();
int nameCheck(char*);
char* errorChecker(char*);
char* createCommand(char*, char*);
char* createPUT(char*, char*);
//used by both
int sendMessage(int, char*);
char* readMessage(int, char*);

#endif
