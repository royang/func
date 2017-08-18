#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define CONN_BUF_LEN	1024

void error_die(char *str)
{
	perror(str);
	exit(-1);
}

void *echo_server(void *arg)
{
	int ret, len;
	int *connfd = (int*)(arg);
	char buf[CONN_BUF_LEN];
	struct sockaddr_in cliaddr, localaddr;
	socklen_t sa_len;

	printf("thread @ %d\n", *connfd);

	while(1){
		len = read(*connfd, buf, CONN_BUF_LEN);
		if(len < 0){
			error_die("socket read");
		}else if( len == 0){
			ret = getsockname(*connfd, (struct sockaddr *)&localaddr, &sa_len);
			if(ret < 0){
				error_die("getsockname");
			}

			ret = getpeername(*connfd, (struct sockaddr *)&cliaddr, &sa_len);
			if(ret < 0){
				error_die("getpeername");
			}

			printf("client %s:%d disconnected form %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
//			close(*connfd);
			pthread_exit(0);
		}else{
			buf[len] = 0;
			ret = getsockname(*connfd, (struct sockaddr *)&localaddr, &sa_len);
			if(ret < 0){
				error_die("getsockname");
			}

			ret = getpeername(*connfd, (struct sockaddr *)&cliaddr, &sa_len);
			if(ret < 0){
				error_die("getpeername");
			}

			printf("client %s:%d -> %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
			printf("%s\n", buf);
		}
	}
}

int main()
{
	int listenfd, connfd[5], ret;
	int connfd_cnt=0;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t sa_len = sizeof(cliaddr);
	pthread_t tid;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0){
		error_die("socket");
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port  	= htons(0);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(listenfd, (struct sockaddr *)&servaddr, sa_len);
	if(ret < 0){
		error_die("bind");
	}

	ret = getsockname(listenfd, (struct sockaddr *)&servaddr, &sa_len);
	if(ret < 0){
		error_die("getsockname");
	}
	printf("server %s:%u\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

	ret = listen(listenfd, 5);
	if(ret < 0){
			error_die("listen");
	}

	while(1)
	{
		connfd[connfd_cnt] = accept(listenfd, (struct sockaddr*)&cliaddr, &sa_len);
		if(ret < 0){
			error_die("accept");
		}

		printf("client %s:%d connected @ %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), connfd[connfd_cnt]);
		
		pthread_create(&tid, NULL, &echo_server, &connfd[connfd_cnt]);
		connfd_cnt++;
	}


}
