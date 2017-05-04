#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include "Wrapsock.h"

int Socket(int family, int type, int protocol)
{
	int n;

	if( (n = socket(family, type, protocol)) < 0){
		perror("socket create");
		exit(-1);
	}else{
		return(n);
	}
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if(bind(fd, sa, salen) < 0){
		perror("socket bind");
		exit(-1);
	}
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (connect(fd, sa, salen) < 0){
		perror("socket connect");
		exit(-1);
	}
}

void Listen(int fd, int backlog)
{
	if (listen(fd, backlog) < 0){
		perror("socket listen");
		exit(-1);
	}
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;

again:
	if( (n = accept(fd, sa, salenptr)) < 0){
		if(errno == ECONNABORTED){
			goto again;
		}else{
			perror("socket accept");
			exit(-1);
		}
	}

	return (n);
}

void Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	if(getsockname(fd, sa, salenptr) < 0){
		perror("getsockname");
		exit(-1);
	}
}

void Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	if(getpeername(fd, sa, salenptr) < 0){
		perror("getpeername");
		exit(-1);
	}
}


const char *Inet_ntop(int fd, void *src, char *dst, socklen_t size)
{
	const char *ptr;
	
	if(dst == NULL){
		printf("NULL pointer 3rd argument to inet_ntop\n");
		exit(-1);
	}
	
	if( (ptr = inet_ntop(fd, src, dst, size)) == NULL){
		perror("inet_ntop");
		exit(-1);
	}

	return (ptr);
}

void Inet_pton(int family, const char *src, void *dst)
{
	if(inet_pton(family, src, dst)<0){
		perror("inet_pton error");
		exit(-1);
	}
}


void Sendto(int fd, const void *ptr, size_t nbytes, int flags, const struct sockaddr *sa, socklen_t salen)
{
	if(sendto(fd, ptr, nbytes, flags, sa, salen) != nbytes){
		perror("sendto");
		exit(-1);
	}
}

ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *sa, socklen_t *salenptr)
{
	ssize_t n;
	
	if( (n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0){
		perror("sendto");
		exit(-1);
	}

	return (n);
}






































































