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

#define MAX_USER 100 //최대 유저 수 
int main(int argc, char** argv)
{
	int listenSd, connectSd;
	struct sockaddr_in srvAddr, clntAddr;
	int clntAddrLen, readLen;
	char rBuff[BUFSIZ];
	int maxFd = 0;
	fd_set defaultFds, rFds;
	int res, i;

    int clntSd[MAX_USER],usercnt=0; //클라이언트 정보 저장
	char clientInfo[MAX_USER][30]; //클라이언트 식별자 (IP:port)
	if(argc != 2)
	{
		printf("Usage: %s [Port Number]\n", argv[0]);
		return -1;
	}

	printf("Server start...\n");
	listenSd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(listenSd == -1 ) printf("socket error");

	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(atoi(argv[1]));

	if(bind(listenSd, (struct sockaddr *) &srvAddr,
				 sizeof(srvAddr)) == -1)
		printf("bind error\n");
	if(listen(listenSd, 5) < 0) printf("listen error\n");

	FD_ZERO(&defaultFds);
	FD_SET(listenSd, &defaultFds);
	maxFd = listenSd;

	clntAddrLen = sizeof(clntAddr);
	
	while(1)
	{
		rFds = defaultFds;
		if((res = select(maxFd + 1, &rFds, 0, 0, NULL)) == -1) break;
		for(i=0; i<maxFd+1; i++)
		{
			if(FD_ISSET(i, &rFds))
			{
				if(i == listenSd) //accept a client
				{
					connectSd = accept(listenSd,
						 (struct sockaddr *) &clntAddr,
						 &clntAddrLen);
					if(connectSd == -1)
					{
						printf("Accept Error");
						continue;
					}
					//
                    clntSd[usercnt]=connectSd;
					sprintf(clientInfo[usercnt],"%s:%d",inet_ntoa(clntAddr.sin_addr),ntohs(clntAddr.sin_port));
					usercnt++;
					//connect한 client 정보 저장
                    write(connectSd, "Hello!", sizeof("Hello!"));
					//들어왔을 때 인사
					
					char wBuff[BUFSIZ];
						
					sprintf(wBuff, "Client (%s) 님이 들어왔습니다.\n",clientInfo[usercnt-1]); // wBuff에 입장한 클라이언트 정보 저장
					printf("Client (%s) 님이 들어왔습니다.\n",clientInfo[usercnt-1]); //입장할 시 메시지

					for(int j=0;j<usercnt-1;j++){// 현재 입장한 client 제외
						write(clntSd[j],wBuff,strlen(wBuff)); 
					}

					FD_SET(connectSd, &defaultFds);
					if(maxFd < connectSd){
						maxFd = connectSd;							
                    }
				}
				else // IO
				{
					int clientNum=i-4;// 현재 클라이언트 번호 (0-indexed)
					readLen = read(i, rBuff, sizeof(rBuff)-1);
					if(readLen <= 0) 
					{
						char wBuff[BUFSIZ];
						
						sprintf(wBuff, "Client (%s) 님이 나갔습니다.\n",clientInfo[clientNum]); // wBuff에 퇴장한 클라이언트 정보 저장
						printf("%s\n",wBuff);
						printf("Client (%s) 님이 나갔습니다.\n",clientInfo[clientNum]); //퇴장할 시 메시지

						for(int j=0;j<usercnt;j++){
							if(j==clientNum)// 퇴장한 client
								continue;
							write(clntSd[j],wBuff,strlen(wBuff)); // 그 외 client
						}
						FD_CLR(i, &defaultFds);
						close(i);
						continue;
					}
					rBuff[readLen] = '\0';

					char wBuff[BUFSIZ];
					sprintf(wBuff,"Client %s: %s",clientInfo[clientNum],rBuff); // wBuff에 채팅 친 클라이언트 정보와 메시지 저장
					printf("%s\n",wBuff);

					for(int j=0;j<usercnt;j++){
						if(j==clientNum)// 현재 채팅 친 client
							continue;
						write(clntSd[j],wBuff,strlen(wBuff)); // 그 외 client에게 정보 전송
					}
				}
			}
		}
	}
	close(listenSd);
	return 0;
}

