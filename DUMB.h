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

int sendMessage(int, char*);
char* readMessage(int, char*);

#endif
