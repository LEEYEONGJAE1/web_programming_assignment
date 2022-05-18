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
  char cBuf[BUF_SIZE];  
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
      memset(cBuf, 0, MSG_SIZE);
      iRtn = read(0, cBuf, MSG_SIZE);      
      cBuf[iRtn - 1] = 0;
      write(iFd, cBuf, MSG_SIZE);
      printf("[Send MSG]: [%s]\n", cBuf);    
    }
    if(0 == (FD_ISSET(iFd, &fdRead) ))
    {
      memset(cBuf, 0, MSG_SIZE);
      iRtn = read(iFd, cBuf, MSG_SIZE);      
      printf("[Server]: [%s]\n", cBuf);    
    }
    if(0 == strncmp(cBuf, MSG_END, strlen(MSG_END)))
    {
      break;
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
