#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#include "../wrap/Wrapsock.h"
#include "../wrap/Wrapunix.h"

#define MAXLINE		4096

int main(int argc, char **argv)
{
	int 				connfd,n;
	char 				recvline[MAXLINE + 1];
	struct sockaddr_in 	servaddr,getaddr;
	socklen_t     		len = sizeof(servaddr),len_get = sizeof(getaddr);

	if(argc != 4)
		printf("usage: client <IPaddress> <Port> <message>");

	connfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port	= htons(atoi(argv[2]));
	Inet_pton( AF_INET , argv[1] , &servaddr.sin_addr);

	Sendto(connfd, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&servaddr, len);

	n = Recvfrom(connfd, recvline, MAXLINE, 0, (struct sockaddr *)&getaddr, &len_get);
	recvline[n] = 0;
	printf("%s:%d--%s\n",inet_ntoa(getaddr.sin_addr),ntohs(getaddr.sin_port),recvline);
	fflush(stdout);

	exit(0);
}

