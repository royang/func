#ifndef _WRAPUNIX_H_
#define _WRAPUNIX_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef	void	Sigfunc(int);	/* for signal handlers */

void Write(int fd, void *ptr, size_t nbytes);
void Close(int fd);
char *Fgets(char *ptr, int n, FILE *stream);
void Fputs(const char *ptr, FILE *stream);
ssize_t Writen(int fd, const void * vptr, size_t n);
ssize_t Read(int fd, void *vptr, size_t maxlen);
pid_t Fork(void);
Sigfunc * Signal(int signo, Sigfunc *func);









#endif
