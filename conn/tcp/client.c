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


void echo_client(FILE *fp,int sockfd)
{
	char sendline[MAXLINE],recvline[MAXLINE];
	ssize_t n;

	while(1) {
		n = Read(sockfd, recvline, MAXLINE);
		
		if(n){
			recvline[n] = 0;
			printf("%s",recvline);		
		}else{
			printf("Read 0 bytes connect lost\n");
			close(sockfd);
			break;
		}
	}	
}

int main(int argc, char **argv)
{
	int 				connfd;
	struct sockaddr_in 	servaddr;

	if(argc != 3)
		printf("usage: client <IPaddress> <Port>");

	connfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port	= htons(atoi(argv[2]));
	Inet_pton( AF_INET , argv[1] , &servaddr.sin_addr);
	
	Connect(connfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	echo_client(stdin, connfd);

	printf("client exit 0\n");
	exit(0);
}


