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
  int serverSd;
  struct sockaddr_in serverAddr;
  char rBuff[BUFSIZ];  
  fd_set defaultFds, rFds;
  int res, len;

  //소켓
  serverSd = socket(AF_INET, SOCK_STREAM, 0);
  if(serverSd==-1) err_proc();
  if(argc != 3) {
		printf("Usage: %s [IP Address] [Port]\n", argv[0]);
	
	}
  
  //address 설정
  memset(&serverAddr,0,sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
  serverAddr.sin_port = htons(atoi(argv[2]));

  //connect
  if(connect(serverSd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))==-1)
  {
    close(serverSd);
    err_proc();
  }

  FD_ZERO(&defaultFds);
	FD_SET(0, &defaultFds);
  FD_SET(serverSd,&defaultFds);

  while(1)
  {
    rFds=defaultFds;     
    
    if((res = select(serverSd+1, &rFds, 0, 0, NULL)) == -1) break;

    if(FD_ISSET(0, &rFds))
    {
      len = read(0, rBuff, BUFSIZ-1);      
      rBuff[len - 1] = 0;
      write(serverSd, rBuff, BUFSIZ-1);    
    }
    if(FD_ISSET(serverSd, &rFds))
    {
      len = read(serverSd, rBuff, BUFSIZ-1);      
      printf("%s\n", rBuff);    
    }
  }
  close(serverSd);
  return 0;
}

void err_proc()
{
	fprintf(stderr,"Error: %s\n", strerror(errno));
	exit(errno);
} 
