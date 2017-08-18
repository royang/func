/*
 * server.c
 *
 *  Created on: Jun 29, 2016
 *      Author: root
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#include "../wrap/Wrapsock.h"
#include "../wrap/Wrapunix.h"

#define MAXLINE	4096

void format_printf(char *buf, int len)
{
	int i=0;

	for(i=0; i<len; i++){
		switch(buf[i]){
			case '\n': printf("\\n\r\n");
				break;
			case '\r': printf("\\r");
				break;
			default:	printf("%c",buf[i]);
				break;
		}
	}
}

void echo_server(int *fd)
{
	char buff[MAXLINE];
	int n;
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	
	while(1){
		n = read(*fd, buff, MAXLINE);
		if(n < 0){

		}else if(n == 0){
			bzero(&cliaddr, sizeof(cliaddr));
			Getpeername(*fd, (struct sockaddr *)&cliaddr, &clilen);
			printf("client %s:%d disconnected\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
			Close(*fd);
			exit(0);
		}else{
			buff[n] = 0;
			bzero(&cliaddr, sizeof(cliaddr));
			Getpeername(*fd, (struct sockaddr *)&cliaddr, &clilen);
			printf("client %s:%d : ", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
			format_printf(buff, n);
	//		printf("%s\n",buff);
		}
	}

	Close(*fd);	
}


int main(int argc, char **argv)
{	
	int 				listenfd, connfd, ret;	
	struct sockaddr_in 	servaddr, cliaddr;	
	socklen_t 			salen = sizeof(cliaddr);	
	pthread_t			tid;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);	

	bzero(&servaddr,sizeof(servaddr));	
	servaddr.sin_family 		= 	AF_INET;	
	servaddr.sin_addr.s_addr 	= 	htonl(INADDR_ANY);
	servaddr.sin_port			= 	htons(9999);	
	Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	Listen(listenfd, 5);

	while(1){		
		bzero(&cliaddr,sizeof(cliaddr));	
		
		connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &salen);
		printf("client %s:%hu connected\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

		
		ret = pthread_create(&tid, NULL, &echo_server, &connfd);
		if(ret < 0){
			perror("thread create");
			Close(connfd);
		}
	}
	Close(listenfd);
	return 0;
} 
