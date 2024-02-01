#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>

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
		0x05,
		0
	};
	int addr_len = sizeof(addr);
	setup_sock(sock_fd, &addr, addr_len);

	int size = 1;
	struct pollfd *pfds;
	pfds = (struct pollfd *)calloc(size,sizeof(struct pollfd));
	pfds[0].fd = sock_fd;
	pfds[0].events = POLLIN;
	char rbuf[100];
	int rdy;
	//int index, last_index = 2;
	while (1) {
		if((rdy = poll(pfds,size,-1)) != 0){
			printf("nb event : %d\n", rdy);
			//printf("index : %d, last_index : %d\n", index, last_index);
			if(rdy == -1){
				perror("shit man\n");
				exit(1);
			}
			
			if(pfds[0].revents == POLLIN){
				size++;
				pfds = (struct pollfd *)reallocarray(pfds,size, sizeof(struct pollfd));
				if((pfds[size-1].fd = accept(sock_fd,(struct sockaddr *) &addr,(unsigned int *) &addr_len)) == -1) {
    					perror("Error linked list\n");
					exit(1);
				}
				pfds[size-1].events = POLLIN;
				printf("new connection\n");
				rdy--;
			}
			
			printf("%d\n",rdy);
			if(rdy>0){
				for(int i=1;i<size;i++){
					printf("wassup\n");
					int bc = read(pfds[i].fd,&rbuf, sizeof(rbuf));
					if(bc == -2){
						perror("read failed");
						exit(1);
					}else if (bc > 0){
						printf("%s",rbuf);
						rdy--;
					}
				}
			}
			printf("out\n");
		} 

 	}

	return 0;
}

