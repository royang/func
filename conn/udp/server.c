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

#include "../wrap/Wrapsock.h"
#include "../wrap/Wrapunix.h"

#define MAXLINE	4096

int main(int argc, char **argv)
{	
	int 				sockfd,n;
	struct sockaddr_in 	servaddr;
	socklen_t 			len = sizeof(servaddr);
	char 				msg[MAXLINE];

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family			= AF_INET;
	servaddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	servaddr.sin_port			= htons(9999);
	Bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	while(1){
		n = Recvfrom(sockfd, msg, MAXLINE, 0, (struct sockaddr *)&servaddr, &len);
		msg[n] = 0;

		printf("from client %s:%d get %d msg : %s\n",inet_ntoa(servaddr.sin_addr),ntohs(servaddr.sin_port),n,msg);
		fflush(stdout);
		Sendto(sockfd, msg, n, 0, (struct sockaddr *)&servaddr, len);

		
	}
	
}
