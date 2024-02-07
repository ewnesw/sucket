#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>


int main(int argc, char *argv[])
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1){
		perror("error lol");
		exit(1);
	}

 	struct sockaddr_in addr = {
		AF_INET,
		0x05,
		0
	};

	if(connect(sock,  (struct sockaddr *) &addr, sizeof(addr)) == -1){
		perror("ah fuck\n");
		exit(1);
	}
     	printf("connected\n");

	struct pollfd *pfds;
	pfds = (struct pollfd *)malloc(2 * sizeof(struct pollfd)); 
	pfds[0].fd = 0;
	pfds[1].fd = sock;
	pfds[0].events = pfds[1].events = POLLIN; 
	char rbuf[100];
	int rdy;
	while (1) {
		if((rdy = poll(pfds,2,-1)) != 0){
			printf("%d\n", rdy);
			if(rdy == -1){
				perror("shit man\n");
				exit(1);
			}

			for(int i=0;i<2;i++){
				if (rdy>0) {
					int bc = read(pfds[i].fd,&rbuf, sizeof(rbuf));
					if(bc==-1){
						perror("read failed");
						exit(1);
					}else if(bc > 0){
						printf("%s\n",rbuf);
						write(sock, &rbuf,sizeof(rbuf));
						rdy--;
					}
    				}else{
					break;
				}
			}
		} 
	}
	return 0;
}
