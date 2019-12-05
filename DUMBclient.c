#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<netdb.h>

int main(int argc, char** argv){
  struct addrinfo hints, *addrinfo;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo = getaddrinfo(argv[1], argv[2], &hints, &addrinfo);
  int sockfd = socket(addrinfo->ai_family, addrinfo->ai_socktype, 0);
  connect(sockfd, addrinfo->ai.addr, addrinfo->ai.addrlen);
}
