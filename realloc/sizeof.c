#include <stdlib.h>

#include <stdio.h>

int main()
{
	struct blob{
		int a;
		char b;
		void *buf;
		char str[12];
	}new, *buf;
	int i;
	 
	printf("sizeof(new) = %lu, sizeof(&new) = %lu\n", sizeof(new), sizeof(&new));
	buf = &new;
	printf("sizeof(buf->buf) = %lu, sizeof(buf->str) = %lu\n", sizeof(buf->buf), sizeof(buf->str));
	buf->buf = (int *)malloc(12);
	if(!buf->buf){
		perror("malloc");
		return -1;
	}
	printf("char *%p, addr %p\n", (char *)buf->buf, &(buf->buf));
	printf("sizeof(new) = %lu, sizeof(buf) = %lu, sizeof(buf->buf) = %lu\n", sizeof(new), sizeof(buf), sizeof(buf->buf));
}
