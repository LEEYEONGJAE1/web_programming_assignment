#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <arpa/inet.h>

void err_proc();
int main(int argc, char** argv)
{
	int clntSd;
	struct sockaddr_in clntAddr;
	int clntAddrLen, readLen, recvByte, maxBuff, res;
	char wBuff[BUFSIZ];
	char rBuff[BUFSIZ];

	if(argc != 3) {
		printf("Usage: %s [IP Address] [toPort]\n", argv[0]);
	}
	clntSd = socket(AF_INET, SOCK_STREAM, 0);
	if(clntSd == -1) err_proc();
	printf("==== client program =====\n");

	memset(&clntAddr, 0, sizeof(clntAddr));
	clntAddr.sin_family = AF_INET;
	clntAddr.sin_addr.s_addr = inet_addr(argv[1]);
	clntAddr.sin_port = htons(atoi(argv[2]));

	if(connect(clntSd, (struct sockaddr *) &clntAddr,
			    sizeof(clntAddr)) == -1)
	{
		close(clntSd);
		err_proc();	
	}

	int inputSd=0;
	fd_set fd;
    FD_ZERO(&fd);  
    FD_SET(0, &fd);
	FD_SET(clntSd, &fd); 
	maxBuff = BUFSIZ-1;
	while(1)
  	{
		     
		if((res = select(clntSd + 1, &fd, 0, 0, NULL)) == -1) break;

		if(FD_ISSET(0, &fd))
		{
			int temp = read(0, wBuff, BUFSIZ-1);      
			wBuff[temp] = '\0';
			write(clntSd, wBuff, maxBuff);  
		}
		if(FD_ISSET(clntSd, &fd)){
			
			recvByte = read(clntSd,rBuff,maxBuff);
			rBuff[recvByte] = '\0';
			printf("Server: %s\n", rBuff);
		}
	}

	printf("END ^^\n");
	close(clntSd);

	return 0;
}

void err_proc()
{
	fprintf(stderr,"Error: %s\n", strerror(errno));
	exit(errno);
} 
