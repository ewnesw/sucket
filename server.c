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


#define PORT 4070
#define IPV4_ADDR "127.0.0.1"


int init_sock(){

	int sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if(sock_fd == -1){
		perror("error lol");
		exit(1);
	}

	return sock_fd;
}

void setup_sock(int sock, struct sockaddr_in * addr,int addr_len){
	if(bind(sock,(struct sockaddr *) addr, addr_len) == -1){
		perror("error lol");
		exit(1);
	}

	int bg_size = 100;
	if(listen(sock, bg_size) == -1){
		perror("error lol");
		exit(1);
	}
}


int main(int argc, char *argv[])
{
 	int sock_fd = init_sock();
	
	struct sockaddr_in addr = {
		AF_INET,
		htons(PORT)
	};

	inet_pton(AF_INET, IPV4_ADDR, &addr.sin_addr);

	int addr_len = sizeof(addr);
	setup_sock(sock_fd, &addr, addr_len);

	int size = 3;
	struct pollfd *pfds;
	pfds = (struct pollfd *)calloc(size,sizeof(struct pollfd));
	pfds[0].fd = sock_fd;
	pfds[0].events = POLLIN;
	char rbuf[100];
	int rdy;
	int index = 1;
	while (1) {
		if((rdy = poll(pfds,size,-1)) != 0){
			if(rdy == -1){
				perror("shit man\n");
				exit(1);
			}
			
			if(pfds[0].revents == POLLIN){
				if((pfds[index].fd = accept(sock_fd,(struct sockaddr *) &addr,(unsigned int *) &addr_len)) == -1) {
    					perror("Error accept\n");
					exit(1);
				}
				pfds[index].events = POLLIN;
				printf("new connection\n");
				index++;
				rdy--;
			}			
			if(rdy>0){
				for(int i=1;i<size;i++){
					if(pfds[i].revents == POLLIN){
						int bc = read(pfds[i].fd,&rbuf, sizeof(rbuf));
						if(bc == -1){
							perror("read failed");
							exit(1);
						}else if (bc > 0){
							printf("%s%d\n",rbuf,bc);
							for (int j=1;j<size;j++) {
      								if(j!=i){
									printf("%ld\n",write(pfds[j].fd,&rbuf,sizeof(rbuf)));
								}
      							}
							rdy--;
						} else if  (bc == 0){
							printf("connection closed\n");
							rdy--;
						}
					}
				}
			}
		} 

 	}

	return 0;
}

