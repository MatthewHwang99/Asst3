#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<netdb.h>
#include<string.h>

int main(int argc, char** argv){
  struct addrinfo hints, *addrinfo;
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
  printf("%s\n", buffer);
  return 0;
}
