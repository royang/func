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

#include <time.h>

#include "../wrap/Wrapsock.h"
#include "../wrap/Wrapunix.h"

#define MAXLINE	4096

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

//	while( (pid = waitpid(-1, &stat, WNOHANG) ) > 0)
	while( (pid = wait(&stat) ) > 0)
		printf("child %d terminated\n", pid);

	return;
}

void echo_server(void* connfd)
{
	char buff[MAXLINE]="this is a demo" ;
	int n;
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	int *fd;
	fd= connfd;
	
	while(1){
//		n = Read(fd, buff, MAXLINE);
//		if(n == 0){
//			bzero(&cliaddr, sizeof(cliaddr));
//			Getpeername(fd, (struct sockaddr *)&cliaddr, &clilen);
//			printf("client %s:%d disconnected\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
//			Close(fd);
//			exit(0);
//		}
		
//		if(sscanf(buff, "%d %d", &arg1, &arg2) == 2)
//			snprintf(buff, MAXLINE, "%d + %d = %d\n", arg1, arg2, arg1+arg2);
//		else
//			snprintf(buff, MAXLINE, "%s", "input error\n");
		
		Write(*fd, buff, strlen(buff));
		sleep(1);
	}

	Close(fd);	
}


int main(int argc, char **argv)
{	
	int 				listenfd,connfd,ret;	
	struct sockaddr_in 	servaddr,cliaddr;	
	socklen_t 			salen = sizeof(cliaddr);	
	pthread_t			tid;


	listenfd = Socket(AF_INET, SOCK_STREAM, 0);	

	bzero(&servaddr,sizeof(servaddr));	
	servaddr.sin_family 		= 	AF_INET;	
	servaddr.sin_addr.s_addr 	= 	htonl(INADDR_ANY);
	servaddr.sin_port			= 	htons(9999);	
	Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	Listen(listenfd, 5);

	Signal(SIGCHLD, sig_chld);

	while(1){		
		bzero(&cliaddr,sizeof(cliaddr));	
		
		connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &salen);
		printf("client %s:%hu connected\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

		
		ret = pthread_create(&tid, NULL, (void *)&echo_server, (void *)&connfd);
		if(ret < 0){
			perror("thread create");
			Close(connfd);
		}
	}
	Close(listenfd);
	return 0;
} 
