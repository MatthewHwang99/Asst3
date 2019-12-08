#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<netdb.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
//#define h_addr h_addr_list[0]

void test_func(int);
int commandCheck(char*);
void help();
void quit(int);

void quit(int socket){
  shutdown(socket, 2);
  return 0;
}

void help(){
  printf("List of commands:\n");
  printf("--help\n--quit\n--create\n--open\n--next\n--put\n--delete\n--close\n");
  return 0;
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
  /*struct addrinfo hints, *addrinfo;
  char buffer[1024] = {0};
  int valread;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo = getaddrinfo(argv[1], argv[2], &hints, &addrinfo);
  int sockfd = socket(addrinfo->ai_family, addrinfo->ai_socktype, 0);
  connect(sockfd, addrinfo->ai.addr, addrinfo->ai.addrlen);
  send(sockfd, HELLO, strlen(HELLO), 0);
  printf("HELLO message sent\n");
  valread = read(sockfd, buffer, 1024);
  printf("%s\n", buffer);*/
  char buffer[1024] = {0};
  char command[1024] = {0};
  if(argc!=3){
  	printf("Error: Please input an IP address or hostname followed by a port number.\n");
  	return 0;
  }
  
  int port = atoi(argv[2]);
  //struct hostent* server = gethostbyname(argv[1]);
  
  struct sockaddr_in server_addr, client;
  int sd, connection, validAddr, valread;
  char* buffer[1024] = {0};
  
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
  send(sd, "HELLO", strlen("HELLO"), 0);
  read(sd, buffer, 1024);
  if(strcmp(buffer, "HELLO DUMBv0 ready!") != 0){
    //CLOSE CONNECTION
    printf("Error: Failed to get the correct initial response from the server."\n);
    shutdown(sd, 2);
    return 0;
  }

  printf("%s\n", buffer);
  while(1){
    //want to stay in this loop until we recieve Goodbye command
    printf("Enter a command:\n>");
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
      
    }else if(commandCheck(command) == 3){
      //open
    }else if(commandCheck(command) == 4){
      //next
    }else if(commandCheck(command) == 5){
      //put
    }else if(commandCheck(command) == 6){
      //delete
    }else if(commandCheck(command) == 7){
      //close
    }else if(commandCheck(command) == -1){
      //Not a valid command or messed up the correct sytanx
    }
    
  }


  return 0;
}


