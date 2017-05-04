#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
	int fd;
	int offset;

	fd = creat("file.hole", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(fd < 0)
	{
		perror("create file :");
		return -1;
	}

	if(write(fd, "abcdefghij", 10) != 10)
		printf("buf write error\n");
	
	offset = lseek(fd, 0, SEEK_CUR);
	if(offset < 0)
	{
		perror("lseek : ");
		return -1;
	}
	else
	{
		printf("file.hole now offset : %d\n", offset);
	}

	offset = lseek(fd, 16384, SEEK_SET);
	if(offset == -1)
	{
		perror("lseek : ");
		return -1;
	}

	if(write(fd, "ABCDEFGHIJ", 10) != 10)
	{
		perror("wirte : ");
		return -1;
	}
	return 0;
}
