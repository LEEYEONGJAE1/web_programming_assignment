#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
void errProc();
void errPrint();
void child_process_handler(int signo){ //새로 추가한 부분
	pid_t pid;
	int status;
	while((pid= waitpid(-1,&status,WNOHANG)) > 0 ){
		printf("child process %d terminated\n", pid);
	}
	return;
}
int main(int argc, char** argv)
{
	int srvSd, clntSd;
	struct sockaddr_in srvAddr, clntAddr;
	int clntAddrLen, readLen, strLen;
	char rBuff[BUFSIZ];
	pid_t pid;
	signal(SIGCHLD,(void *) child_process_handler); // SIGCHLD 핸들러 추가
	if(argc != 2) {
		printf("Usage: %s [port] \n", argv[0]);
		exit(1);
	}
	printf("Server start...\n");

	srvSd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(srvSd == -1 ) errProc();

	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(atoi(argv[1]));

	if(bind(srvSd, (struct sockaddr *) &srvAddr, sizeof(srvAddr)) == -1)
		errProc();
	if(listen(srvSd, 5) < 0)
		errProc();
	clntAddrLen = sizeof(clntAddr);
	while(1)
	{
		clntSd = accept(srvSd, (struct sockaddr *)
					 &clntAddr, &clntAddrLen);
		if(clntSd == -1) {
			errPrint();
			continue;	
		}
		printf("client %s:%d is connected...\n",
			inet_ntoa(clntAddr.sin_addr),
			ntohs(clntAddr.sin_port));	
		pid = fork();
		if(pid == 0) { /* child process */
			close(srvSd);
			while(1) {
				readLen = read(clntSd, rBuff, sizeof(rBuff)-1);
				if(readLen == 0) break;
				rBuff[readLen] = '\0';
				printf("Client(%d): %s\n",
					ntohs(clntAddr.sin_port),rBuff);
				write(clntSd, rBuff, strlen(rBuff));
			}
			printf("Client(%d): is disconnected\n",
				ntohs(clntAddr.sin_port));
			close(clntSd);
			return 0;
		}	
		else if(pid == -1) errProc("fork");
		else { /*Parent Process*/
			close(clntSd);
		}	
	}
	close(srvSd);
	return 0;
}

void errProc(const char *str)
{
    fprintf(stderr,"%s: %s \n",str, strerror(errno));
    exit(1);
}

void errPrint(const char *str)
{
	fprintf(stderr,"%s: %s \n",str, strerror(errno));
}
