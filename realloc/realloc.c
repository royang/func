#include <stdio.h>
#include <stdlib.h>

struct blob_buf{
	int a;
	char b;
	void *buf;
};

int main(int argc, char *argv[])
{
	struct blob_buf *buf,*new;

	buf = (struct blob_buf *)malloc(sizeof(struct blob_buf));
	if(!buf)
	{
		perror("malloc");
		return -1;
	}

	printf("before realloc :  buf address %p, buf->buf address %p buf size %lu, buf->buf size %lu\n", buf, &buf->buf, sizeof(buf), sizeof(buf->buf));

	new = realloc(buf->buf, 128);
	if(!new){
		perror("realloc");
		return -1;
	}
	buf->buf = new;

	printf("after  realloc :  buf address %p, buf->buf address %p buf size %d, buf->buf size %d\n", buf, &buf->buf, sizeof(buf), sizeof(buf->buf));

}
