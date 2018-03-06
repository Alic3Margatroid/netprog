#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#define SERV_PORT 8000
#define MAXLINE 255

int compare_addr(struct sockaddr_in sa1, struct sockaddr_in sa2){
	return (sa1.sin_addr.s_addr == sa2.sin_addr.s_addr) && (sa1.sin_port == sa2.sin_port) ? 0 : -1;
}

int notin(struct sockaddr_in client[], struct sockaddr_in cliaddr, int top){
	int i;
	for (i = 0; i < top; i++){
		if (compare_addr(cliaddr, client[i]) == 0)
			return 0;
	}
	return 1;
}

int main()
{
   	int sockfd, n, top, i;
   	socklen_t len;
   	char mesg[MAXLINE]; 
   	struct sockaddr_in servaddr, cliaddr, client[100];
 
   	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
   	memset(&servaddr, 0, sizeof(servaddr)); 
   	servaddr.sin_family = AF_INET; 
   	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
   	servaddr.sin_port = htons(SERV_PORT); 
   	if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))>=0){
	    printf("Server is running at port %d\n", SERV_PORT);
	}
  	else{
    	perror("bind failed");
    	return 0;
    }

   	for ( ; ; ) { 
		len = sizeof(cliaddr); 
		n = recvfrom(sockfd, mesg, MAXLINE, 0, (struct sockaddr *) &cliaddr, &len); 
		mesg[n] = '\0';
		puts(mesg);
		printf("Receiving data :%s",mesg );
		if(notin(client, cliaddr, top)){
			client[top] = cliaddr;
			top ++;
		}
		for (i = 0; i < top; i++){
			if(compare_addr(cliaddr, client[i]) != 0){
				if (strcmp(mesg, "d8578edf8458ce06fbc5bb76a58c5ca4") == 0){
					continue;
				}
				sendto(sockfd, mesg, n, 0, (struct sockaddr *) &client[i], len); 
			}
		}
   	}
	close(sockfd);
	return 0;
}