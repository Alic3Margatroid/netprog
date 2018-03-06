#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERV_PORT 8000
#define MAXLINE 255
int main(){
  fd_set master, read_fds;
  int sockfd, n, from_len, fdmax, i;
  struct sockaddr_in servaddr, from_socket;
  socklen_t addrlen = sizeof(from_socket);
  char sendline[MAXLINE], recvline[MAXLINE + 1];

  servaddr.sin_family = AF_INET; 
  servaddr.sin_port = htons(SERV_PORT); 
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  FD_SET(fileno(stdin), &master);
  FD_SET(sockfd, &master);
  //printf("%d, %d\n", fileno(stdin), sockfd);
  fdmax = sockfd > fileno(stdin) ? sockfd : fileno(stdin);
  sendto(sockfd, "d8578edf8458ce06fbc5bb76a58c5ca4", strlen("d8578edf8458ce06fbc5bb76a58c5ca4"), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

  for ( ; ; ) {
    read_fds = master; 
    if(select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1){
      perror("select");
      exit(1);
    }

    if (FD_ISSET(sockfd, &read_fds)){
        printf("sockfd is set \n");
        n = recvfrom(sockfd, recvline, MAXLINE, 0,  (struct sockaddr *) &from_socket, &addrlen);
        recvline[n] = '\0';
        printf("Server: %s %d \nMessage:%s", inet_ntoa(from_socket.sin_addr), htons(from_socket.sin_port), recvline);
    }

    if (FD_ISSET(fileno(stdin), &read_fds)){
        printf("stdin is set \n");
        fgets(sendline, MAXLINE, stdin);
        sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    }    
  }
  
  close(sockfd);
  return 0;
}