#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#include "../wrap/Wrapsock.h"
#include "../wrap/Wrapunix.h"

#define MAXLINE		4096

#define SERVER_IP	"139.199.20.131"
#define SERVER_PORT	9999

int connfd;

void *client_heartbeat(void *arg)
{
	char	sendline[MAXLINE] = "heartbeat";
	struct 	sockaddr_in 	*servaddr = arg;
	socklen_t len = sizeof(struct 	sockaddr_in);
	
	while(1)
	{
		Sendto(connfd, sendline, strlen(sendline), 0, (struct sockaddr *)servaddr, len);
		sleep(1);
	}
}

int main(int argc, char **argv)
{
	int 				n,ret;
	char 				recvline[MAXLINE + 1];
	struct sockaddr_in 	servaddr,getaddr;
	socklen_t 			len_get = sizeof(getaddr);
	pthread_t 			pid;

	connfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port	= htons(SERVER_PORT);
	Inet_pton( AF_INET , SERVER_IP , &servaddr.sin_addr);
	
	ret = pthread_create(&pid, NULL, &client_heartbeat, &servaddr);
	if(ret)
	{
		perror("new thread create :");
		exit(-1);
	}

	while(1){
		n = Recvfrom(connfd, recvline, MAXLINE, 0, (struct sockaddr *)&getaddr, &len_get);
		recvline[n] = 0;
		printf("%s:%d--%s\n",inet_ntoa(getaddr.sin_addr),ntohs(getaddr.sin_port),recvline);
		fflush(stdout);
	}
	exit(0);
}

