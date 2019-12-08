#include"DUMB.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<netdb.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>

int commandCheck(char*);
void help();
void quit(int);
int nameCheck(char*);

int nameCheck(char* name){
  int size = strlen(name);
  char first = name[0];
  if(size >= 5 && size <= 25 && ((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z'))){
    //valid name
    return 0;
  }
  //invalid name
  return 1;
}

void quit(int socket){
  shutdown(socket, 2);
  return;
}

void help(){
  printf("List of commands:\n");
  printf("--help\n--quit\n--create\n--open\n--next\n--put\n--delete\n--close\n");
  return;
}

int commandCheck(char* command){
  if(strcmp(command, "help")){
    return 0;
  }else if(strcmp(command, "quit")){
    return 1;
  }else if(strcmp(command, "create")){
    return 2;
  }else if(strcmp(command, "open")){
    return 3;
  }else if(strcmp(command, "next")){
    return 4;
  }else if(strcmp(command, "put")){
    return 5;
  }else if(strcmp(command, "delete")){
    return 6;
  }else if(strcmp(command, "close")){
    return 7;
  }
  return -1;
}

int main(int argc, char** argv){
  char buffer[1024] = {0};
  char *command;
  char arg[1024] = {0};
  if(argc!=3){
  	printf("Error: Please input an IP address or hostname followed by a port number.\n");
  	return 0;
  }
  
  int port = atoi(argv[2]);
  //struct hostent* server = gethostbyname(argv[1]);
  
  struct sockaddr_in server_addr, client;
  int sd, connection, validAddr, valread;
  //char buffer[1024] = {0};
  
  //establish socket
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
  	printf("Socket creation failed.\n");
  	return 0;
  }
  
  memset(&server_addr, '0', sizeof(server_addr));
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(port);
  
  if((connection = connect(sd, (struct sockaddr*)&server_addr, sizeof(server_addr))) == -1){
  	printf("Connection failed.\n");
  }
 
  //E.0 HELLO

  char* msg = "HELLO\0";
  sendMessage(sd, msg);
  readMessage(sd, buffer);
  
  if(strcmp(buffer, "HELLO DUMBv0 ready!") != 0){
    //CLOSE CONNECTION
    printf("Error: Failed to get the correct initial response from the server.\n");
    shutdown(sd, 2);
    return 0;
  }

  printf("%s\n", buffer);
  while(1){
    //want to stay in this loop until we recieve Goodbye command
    printf("> ");
    scanf("%s", command);
    if(commandCheck(command) == 0){
      //help, List of commands
      help();
    }else if(commandCheck(command) == 1){
      //quit
      quit(sd);
      break;
    }else if(commandCheck(command) == 2){
      //create
      printf("%s:> ", command);
      scanf("%s", arg);
      if(nameCheck(arg)){
	printf("Error. %s isn't a valid name.\n", arg);
	continue;
      }
      command = "CREAT ";
      strcat(command, arg);
      sendMessage(sd, command);
    }else if(commandCheck(command) == 3){
      //open
      printf("%s:> ", command);
      scanf("%s", arg);
      if(nameCheck(arg)){
	printf("Error. %s isn't a valid name.\n", arg);
	continue;
      }
      command = "OPNBX ";
      strcat(command, arg);
      sendMessage(sd, command);
    }else if(commandCheck(command) == 4){
      //next
      command = "NXTMG";
      sendMessage(sd, command);      
    }else if(commandCheck(command) == 5){
      //put
      printf("%s:> ", command);
      scanf("%s", arg);
      command = "PUTMG!";
      char* size;
      sprintf(size, "%d", strlen(arg));
      strcat(size, "!");
      strcat(command, size);
      strcat(command, arg);
      sendMessage(sd, command);      
    }else if(commandCheck(command) == 6){
      //delete
      printf("%s:> ", command);
      scanf("%s", arg);
      if(nameCheck(arg)){
	printf("Error. %s isn't a valid name.\n", arg);
	continue;
      }
      command = "DELBX ";
      strcat(command, arg);
      sendMessage(sd, command);
    }else if(commandCheck(command) == 7){
      //close
      printf("%s:> ", command);
      scanf("%s", arg);
      if(nameCheck(arg)){
	printf("Error. %s isn't a valid name.\n", arg);
	continue;
      }
      command = "CLSBX ";
      strcat(command, arg);
      sendMessage(sd, command);
    }else if(commandCheck(command) == -1){
      //Not a valid command or messed up the correct sytanx
      printf("Error. Your command is invalid\n");
      continue;
    }
    
  }


  return 0;
}


