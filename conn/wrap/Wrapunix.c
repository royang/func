
#include <errno.h>
#include <signal.h>

//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <string.h>

#include "Wrapunix.h"


void Write(int fd, void *ptr, size_t nbytes)
{
	if( write(fd, ptr, nbytes) != nbytes){
		perror("write");
		exit(-1);
	}
}

void Close(int fd)
{
	if(close(fd) == -1){
		perror("close");
		exit(-1);
	}
}

char *Fgets(char *ptr, int n, FILE *stream)
{
	char *rptr;

	if( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream)){
		perror("fgets");
		exit(-1);
	}

	return (rptr);
}

void Fputs(const char *ptr, FILE *stream)
{
	if( fputs(ptr, stream) == EOF){
		perror("fputs");
		exit(-1);
	}
}

ssize_t Writen(int fd, const void * vptr, size_t n)
{
	size_t 		nleft;
	ssize_t		nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;

	while(nleft > 0) {
again:
		if( (nwritten = write(fd, ptr, nleft)) <= 0){
			if(errno == EINTR){
				goto again;
			}else{
				perror("writen");
				exit(-1);
			}
		}else{
			nleft -= nwritten;
			ptr += nwritten;
		}
	}
	
	return (n);	
}

ssize_t Read(int fd, void *vptr, size_t maxlen)
{
	char *ptr;
	ssize_t n=0,nreaded=0;

	ptr = vptr;
again:
	n = read(fd, ptr, maxlen);		
	if(n < 0){
		if(errno == EINTR){
			goto again;
		}else{
			perror("Read");
			exit(-1);
		}
	}else if(n == 0){
		nreaded = 0;
	}else{
		nreaded += n;
		ptr += n;
	};

	*ptr = 0;
	return nreaded;			
}

pid_t Fork(void)
{
	pid_t pid;
	if( (pid = fork()) == -1){
		perror("fork");
		exit(-1);
	}

	return pid;
}

Sigfunc * Signal(int signo, Sigfunc *func)
{
	struct sigaction act,oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if(signo == SIGALRM){
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	}else{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}

	if(sigaction(signo, &act, &oact) < 0){
		perror("signal error");
		exit(-1);
	}

	return oact.sa_handler;
}

//void main()
//{
//	int fd, n;
//	char str_write[20] = "0123456789",str_read[30];

//	fd = open("./test.txt",O_RDWR | O_CREAT | O_NOCTTY | O_APPEND);

//	write(fd, str_write, strlen(str_write));
//	
//	bzero(str_read,sizeof(str_read));
//	n = Read(fd, str_read, 30);
//	

//	printf("read %d : %s\n",n,str_read);
//	
//}






































































