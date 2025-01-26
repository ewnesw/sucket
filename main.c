#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "client.c"
#include "server.c"

#define ARGV_SIZE(a) (sizeof(*a) / sizeof(char *)) 

#define DEF_PORT 4080
#define DEF_HOST "127.0.0.1"
#define DEF_BUFFER 100
#define DEF_USERSIZE 2

void helper(){
	printf("sucket usage :\n");
	printf("\n");
	printf("sucket <Command> -<sub-command> <arg>\n");
	printf("Commands :\n");
	printf("server : run server\n");
	printf("client : run client\n");
	printf("\n");
}


int main(int argc, char* argv[])
{
	if (argc==1){
		helper();
		printf("no command passed\n");
		exit(1);
	}

	if(strcmp(argv[1],"server") == 0){
		suck_server(DEF_PORT, DEF_HOST, 3, DEF_BUFFER);
	}else if(strcmp(argv[1],"client") == 0){
		suck_client(DEF_PORT, DEF_HOST,DEF_BUFFER);
	}else{
		helper();
		printf("command doesn't exist");
		exit(1);
	}
	
	return 0;
}

