#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <arpa/inet.h>

int client_sock(int port, char* ipv4_addr){

	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1){
		perror("error lol");
		exit(1);
	}

 	struct sockaddr_in addr = {
		AF_INET,
		htons(port)
	};

	inet_pton(AF_INET, ipv4_addr, &addr.sin_addr);

	int ret = connect(sock,  (struct sockaddr *) &addr, sizeof(addr));
	if( ret == -1){
		perror("ah fuck\n");
		exit(1);
	}
     	printf("connected\n");

	return sock;
}

struct pollfd* client_poll(int sock){
	struct pollfd *pfds;
	pfds = (struct pollfd *)malloc(2 * sizeof(struct pollfd)); 
	pfds[0].fd = 0;
	pfds[1].fd = sock;
	pfds[0].events = pfds[1].events = POLLIN; 

	return pfds;
}

void client_run(struct pollfd * pfds, int csock, int buf_size){
	char rbuf[buf_size];
	int rdy;

	while (1) {
		if((rdy = poll(pfds,2,-1)) != 0){
			if(rdy == -1){
				perror("shit man\n");
				exit(1);
			}

			if (rdy==0) {
				break;
			}
			
			for(int i=0;i<2;i++){
				if (pfds[i].revents==POLLIN){
					int bc = read(pfds[i].fd,&rbuf, sizeof(rbuf));
					if(bc==-1){
						perror("read failed");
						exit(1);
					}else if(bc > 0){
						printf("%s%d",rbuf,bc);
						if(i==0){
							printf("%ld\n", write(csock, &rbuf,sizeof(rbuf)));
						}
						rdy--;
				
					}
				}
			}
		} 
	}
}


void suck_client(int port, char* ipv4_addr, int buf_size){
	int csock = client_sock(port,ipv4_addr);
	struct pollfd *pfds = client_poll(csock);
	
	client_run(pfds,csock,buf_size);
	close(csock);
}
