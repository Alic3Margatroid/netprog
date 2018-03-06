/*
** selectserver.c -- a cheezy multiperson chat server
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define PORT "9034"   // port we're listening on
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int compareaddr(struct sockaddr_in sa1, struct sockaddr_in sa2){
	return (sa1.sin_port == sa2.sin_port) && (sa1.sin_addr.s_addr == sa2.sin_addr.s_addr) ? 0 : -1;
}

int notnull(struct sockaddr_in sa1){

}

int main(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_in remoteaddr; // client address
    struct sockaddr_in remoteset[100];
    int top=0;
    socklen_t addrlen;
    char buf[256];    // buffer for client data
    int nbytes;
    char remoteIP[INET_ADDRSTRLEN];
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;
    struct addrinfo hints, *ai, *p;
    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
        	            close(listener);
            continue;
        }
        break;
    }
    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }
    freeaddrinfo(ai); // all done with this
    // listen
    // if (listen(listener, 10) == -1) {
    //     perror("listen");
    //     exit(3);
    // }
    // add the listener to the master set
    FD_SET(listener, &master);
    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one
    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // // handle new connections
                    // addrlen = sizeof remoteaddr;
                    // newfd = accept(listener,
                    //     (struct sockaddr *)&remoteaddr,
                    //     &addrlen);
                    // if (newfd == -1) {
                    //     perror("accept");
                    // } else {
                    //     FD_SET(newfd, &master); // add to master set
                    //     if (newfd > fdmax) {    // keep track of the max
                    //         fdmax = newfd;
                    //     }
                    //     printf("selectserver: new connection from %s on "
                    //         "socket %d\n",
                    //         inet_ntop(remoteaddr.ss_family,
                    //             get_in_addr((struct sockaddr*)&remoteaddr),
                    //             remoteIP, INET6_ADDRSTRLEN),
                    //         newfd);
                    // }
                } else {
                    // handle data from a client
                    if ((nbytes = recvfrom(listener, buf, sizeof buf, 0, (struct sockaddr*)&remoteaddr, sizeof(remoteaddr))) <= 0) {
                    	                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        remoteset[top] = remoteaddr;
                        top ++;
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        //we got some data from a client
                        for(j = 0; j <= top; j++) {
                            // send to everyone!
                            //if (FD_ISSET(j, &master)) {
                        	if (1){
                                // except the listener and ourselves
                                //if (j != listener && j != i) {
                                    if (sendto(listener, buf, nbytes, 0, (struct sockaddr*)&remoteaddr, sizeof(remoteaddr)) == -1) {
                                        perror("send");
                                    }
                                //}
                            }
                        }
                        // if (FD_ISSET(5, &master)){
                        // 	if(listener != 5 && i != 5){
                        // 		if (send(5, buf, nbytes, 0) == -1) {
                        //         	perror("send");
                        // 		}
                        // 	}
                        // }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    return 0;
}