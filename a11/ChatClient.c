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
  int iFd;
  struct sockaddr_in stAddr;
  int iLen;
  int iRtn;
  char cBuf[BUFSIZ];  
  fd_set fdRead;


  /*** socket ***/
  iFd = socket(AF_INET, SOCK_STREAM, 0);
  if(-1 == iFd)
  {
    perror("socket:");
    return 100;
  }

  /*** structure setting ***/
  stAddr.sin_family = AF_INET;
  stAddr.sin_addr.s_addr = inet_addr(argv[1]);
  stAddr.sin_port = htons(atoi(argv[2]));

  iLen = sizeof(struct sockaddr_in);

  /*** connect ***/
  iRtn = connect(iFd, (struct sockaddr *)&stAddr, iLen);
  if(-1 == iRtn)
  {
    perror("connect:");
    close(iFd);
    return 200;
  }

  while(1)
  {
    FD_ZERO(&fdRead);  
    FD_SET(0, &fdRead);
    FD_SET(iFd, &fdRead);      
    select(iFd+1,&fdRead, 0, 0, 0);

    if(0 == (FD_ISSET(0, &fdRead) ) )
    {
      memset(cBuf, 0, BUFSIZ-1);
      iRtn = read(0, cBuf, BUFSIZ-1);      
      cBuf[iRtn - 1] = 0;
      write(iFd, cBuf, BUFSIZ-1);    
    }
    if(FD_ISSET(iFd, &fdRead))
    {
      memset(cBuf, 0, BUFSIZ-1);
      iRtn = read(iFd, cBuf, BUFSIZ-1);      
      printf("[Server]: [%s]\n", cBuf);    
    }
  }

  

  /*** read & write ***/
  //memset(cBuf, 0, BUF_SIZE);
  //iRtn = read(0, cBuf, BUF_SIZE);
  

  close(iFd);
  return 0;
}

void err_proc()
{
	fprintf(stderr,"Error: %s\n", strerror(errno));
	exit(errno);
} 
