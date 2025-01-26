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

struct sockaddr_in setup_addr(int port, char *ipv4_addr){
	struct sockaddr_in addr_in = {
		AF_INET,
		htons(port)
	};

	inet_pton(AF_INET, ipv4_addr, &addr_in.sin_addr);
	return addr_in;
}


struct pollfd* setup_poll(int sock, int size){
	printf("%d",size);
	struct pollfd *pfds;
	pfds = (struct pollfd *)calloc(size,sizeof(struct pollfd));
	pfds[0].fd = sock;
	pfds[0].events = POLLIN;

	return pfds;
}
void server_run(struct pollfd * pfds, int user_size,int sock, struct sockaddr_in addr, int addr_len, int buf_size){
	char rbuf[buf_size];
	int rdy;
	int index = 1;

	while (1) {
		if((rdy = poll(pfds,user_size,-1)) != 0){
			if(rdy == -1){
				perror("shit man\n");
				exit(1);
			}
			
			if(pfds[0].revents == POLLIN){
				if((pfds[index].fd = accept(sock,(struct sockaddr *) &addr,(unsigned int *) &addr_len)) == -1) {
    					perror("Error accept\n");
					exit(1);
				}
				pfds[index].events = POLLIN;
				printf("new connection\n");
				index++;
				rdy--;
			}			
			if(rdy>0){
				for(int i=1;i<user_size;i++){
					printf("%d : %d\n",i,pfds[i].revents);
					if(pfds[i].revents == POLLIN){
						int bc = read(pfds[i].fd,&rbuf, sizeof(rbuf));
						if(bc == -1){
							perror("read failed");
							exit(1);
						}else if (bc > 0){
							printf("%s%d\n",rbuf,bc);
							for (int j=1;j<user_size;j++) {
      								if(j!=i){
									printf("%ld\n",write(pfds[j].fd,&rbuf,sizeof(rbuf)));
								}
      							}
							rdy--;
						} else if  (bc == 0){
							printf("connection closed\n");
							rdy--;
						}
					}else if (pfds[i].revents==POLLHUP){
						printf("connection closed\n");
						close(pfds[i].fd);
						rdy--;
					}
				}
			}
		} 

 	}

}

void suck_server(int server_port, char *server_ipv4, int user_size, int buf_size){
 	int sock_fd = init_sock();
	struct sockaddr_in addr = setup_addr(server_port, server_ipv4);	
	int addr_len = sizeof(addr);
	setup_sock(sock_fd, &addr, addr_len);
	struct pollfd * pfds = setup_poll(sock_fd, user_size);
	
	server_run(pfds, user_size, sock_fd, addr, addr_len, buf_size);
	close(sock_fd);
}
