#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>

void hexshow(uint8_t *buf, int len)
{
	int i;
	
	printf("Get %d : ", len);
	for(i=0; i<len; i++){
		printf("%02x ", buf[i]);
	}
	printf("\n");
}

int main()
{
	int fd_read, fd_write, ret, len;
	uint8_t buf[512];

	fd_read = open("./test.bin", O_RDONLY);
	if(fd_read < 0){
		perror("file test.bin open");
		exit(-1);
	}

	fd_write = open("./write.bin", O_WRONLY | O_CREAT);
	if(fd_write < 0){
		perror("file write.bin open");
		exit(-1);
	}

	while(1){
		ret = read(fd_read, buf, 16);
		if(ret < 0){
			perror("file read");
			exit(-1);
		}

		hexshow(buf, ret);

		len = write(fd_write, buf, ret);
		if(len < 0){
			perror("file write");
			exit(-1);
		}
	}
}
