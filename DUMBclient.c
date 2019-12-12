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


//int successCheck(char*);
/*Function for when the server returns "OK!"
int successCheck(char* command){
  char* protocol[6];
  int i;
  for(i = 0; i < 5; i++){
    protocol[i] = command[i];
  }
  protocol[6] = '\0';
  if(strcmp(protocol, "CREAT") == 0){
    printf("Box was created successfully.\n");
  }else if(strcmp(protocol, "DELBX") == 0){
    printf("Box was deleted.\n");
  }else if(strcmp(protocol, "OPNBX") == 0){
    printf("Box is now open.\n");
  }else if(strcmp(protocol, "CLSBX") == 0){
    printf("You have closed a box.\n");
  }else if(strcmp(protocol, "NXTMG") == 0){
    printf("");
  }else if(strcmp(protocol, "PUTMG") == 0){
    printf("");
  }
}
*/


int main(int argc, char** argv){
  char buffer[1024] = {0};
  char *command = (char*)malloc(sizeof(char) * 1024);
  char *arg = (char*)malloc(sizeof(char) * 1024);
  if(argc!=3){
  	printf("Error: Please input an IP address or hostname followed by a port number.\n");
  	return 0;
  }
  
  char* port = argv[2];
    
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
  server_addr.sin_port = htons(atoi(port));
  
  if((connection = connect(sd, (struct sockaddr*)&server_addr, sizeof(server_addr))) == -1){
  	char* host = argv[1];
  	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags = AI_ADDRCONFIG | AI_ALL;
	struct addrinfo* ret;
	if(getaddrinfo(host, port, &hints, &ret)!=0){
		printf("Fatal Error: Failed to get ip for given hostname %s\n", host);
		free(host);
		close(sd);
		exit(0);
	}
	
	/*char addrstr[100];
	if(getnameinfo(ret->ai_addr, ret->ai_addrlen, addrstr, 100, NULL, 0, NI_NUMERICHOST)!=0){
		printf("Fatal Error: Failed to get ip for given hostname %s\n", host);
		free(host);
		close(sd);
		exit(0);
	}*/
	
  	if((connection = connect(sd, ret->ai_addr, ret->ai_addrlen)) == -1){
  		printf("Connection failed.\n");
  		exit(0);
  	}
  }
  
 
  //E.0 HELLO

  char* msg = "HELLO\0";
  sendMessage(sd, msg);
  readMessage(sd, buffer);
  
  if(strcmp(buffer, "HELLO DUMBv0 ready!") != 0){
    //CLOSE CONNECTION
    printf("Error: Failed to get the correct initial response from the server.\n");
    close(sd);
    return 0;
  }
	
	char* str;

  printf("%s\n", buffer);
  while(1){
    //want to stay in this loop until we recieve Goodbye command
    printf("> ");
    scanf("%s", command);
    if(strcmp(command, "help") == 0){
      //help, List of commands
      help();
      continue;
    }else if(strcmp(command, "quit") == 0){
      //quit
      str = "GDBYE";
      sendMessage(sd, str);
      close(sd);
      break;
    }else if(strcmp(command, "create") == 0){
      //create
      printf("%s:> ", command);
      scanf("%s", arg);
      if(nameCheck(arg)){
		printf("Error. %s isn't a valid name.\n", arg);
		continue;
      }
      str = "CREAT ";
      char* newcommand = createCommand(str, arg);
            
      sendMessage(sd, newcommand);
    }else if(strcmp(command, "open") == 0){
      //open
      printf("%s:> ", command);
      scanf("%s", arg);
      if(nameCheck(arg)){
		printf("Error. %s isn't a valid name.\n", arg);
		continue;
      }
      str = "OPNBX ";
      char* newcommand = createCommand(str, arg);
      sendMessage(sd, newcommand);
    }else if(strcmp(command, "next") == 0){
      //next
      command = "NXTMG";
      sendMessage(sd, command);      
    }else if(strcmp(command, "put") == 0){
      //put
      printf("%s:> ", command);
      char *i;
      while(*i != '\n'){
	scanf("%c", i);
	strcat(arg, i);
      }
      printf("%s\n", arg);
      command = "PUTMG!";
      char* newcommand = createPUT(command, arg);
      /*char* size;
      sprintf(size, "%d", strlen(arg));
      strcat(size, "!");
      strcat(command, size);
      strcat(command, arg);*/
      printf("%s\n", newcommand);
      sendMessage(sd, newcommand);      
    }else if(strcmp(command, "delete") == 0){
      //delete
      printf("%s:> ", command);
      scanf("%s", arg);
      if(nameCheck(arg)){
		printf("Error. %s isn't a valid name.\n", arg);
		continue;
      }
      str = "DELBX ";
      char* newcommand = createCommand(str, arg);
      sendMessage(sd, newcommand);
    }else if(strcmp(command, "close")){
      //close
      printf("%s:> ", command);
      scanf("%s", arg);
      if(nameCheck(arg)){
	printf("Error. %s isn't a valid name.\n", arg);
	continue;
      }
      str = "CLSBX ";
      char* newcommand = createCommand(str, arg);
      sendMessage(sd, newcommand);
    }else{
      //Not a valid command or messed up the correct sytanx
      printf("Error. Your command is invalid\n");
      continue;
    }
    
    readMessage(sd, buffer);
    
    if(strcmp("OK!", errorChecker(buffer)) == 0){
      if(strcmp(command, "CREAT") == 0){
		printf("Box successfully created.\n");
      }else if(strcmp(command, "OPNBX") == 0){
		printf("Box successfully opened.\n");
      }else if(strcmp(command, "NXTMG") == 0){
		char* token = strtok(buffer, "!");
		int i;
		for(i = 0; i < 2; i++){
	 	 token = strtok(NULL, "!");
		}
		printf("%s\n", token);
      }else if(strcmp(command, "PUTMG") == 0){
		printf("Message successfully put\n");
      }else if(strcmp(command, "DELBX") == 0){
		printf("Successfully deleted a message box.\n");
      }else if(strcmp(command, "CLSBX") == 0){
		printf("Successfully closed box.\n");
      }
    }else{
      printf("%s", errorChecker(buffer));
    }
    	
    //memset(command, 0, strlen(command));
    //memset(arg, 0, strlen(arg));

  }

  return 0;
}


