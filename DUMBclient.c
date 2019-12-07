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
  
  
  return 0;
}


