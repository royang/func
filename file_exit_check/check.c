#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "md5sum.h"

#define PATH_LENGTH_MAX	128
#define READ_DATA_SIZE	1024

int fileSearch(char *basePath, char *header, char *ValidPath)
{
	DIR *dir;
	struct dirent *ptr;

	if((dir = opendir(basePath)) == NULL){
		perror("Open dir");
		exit(-1);
	}

	while((ptr = readdir(dir)) != NULL){
		if(ptr->d_type == 8){
			int len = 0;

			while((header[len] != '\0') && (header[len] != '_')) {
				len++;
			}

			if(strncmp(ptr->d_name, header, len) == 0){
				memset(ValidPath, '\0', PATH_LENGTH_MAX);
				strcat(ValidPath, basePath);
				strcat(ValidPath, "/");
				strcat(ValidPath, ptr->d_name);
				printf("%s matched\n", ValidPath);

				closedir(dir);
				return 0;
			}else{
				printf("%s, %s, %d not matched\n", ptr->d_name, header, len);
			}
		}else{
			printf("%s not file\n", ptr->d_name);
		}
	
	}

	closedir(dir);

	return -1;
}

int  checkMd5Value(char *path)
{
	unsigned char data_buf[1024];
	char path_buf[PATH_LENGTH_MAX];
	char md5_value[33];
	unsigned char md5[16];
	MD5_CTX ctx;
	int fd;
	int nread;
	int i;
	char *token;

	fd = open(path, O_RDONLY);
	if(fd == -1)
	{
		close(fd);
		perror("open");
		exit(EXIT_FAILURE);
	}

	MD5Init(&ctx);
	while(nread = read(fd, data_buf, sizeof(data_buf)), nread > 0)
	{
		MD5Update(&ctx, data_buf, nread);
	}
	MD5Final(&ctx, md5);

	close(fd);

	for(i=0; i<sizeof(md5); i++)
	{
		snprintf(md5_value+i*2, 3, "%02x", md5[i]);
	}
	md5_value[32] = '\0';
	printf("md5_value %s\n", md5_value);

	strcpy(path_buf, path);
	token = strtok(path_buf, "_");
	printf("file name : %s\n", token);
	token = strtok(NULL, "_");
	printf("file Md5 value:%s\n", token);

	return strncmp(md5_value, token, 32);
}

int main(int argc, char *argv[])
{
	char file[PATH_LENGTH_MAX];
	int ret;

	if(argc < 3){
		printf("intput dirent path and keyword\n");
		return -1;
	}

	ret = fileSearch(argv[1], argv[2], file);
	if(ret){
		printf("not matched\n");
		return 0;
	}else{
		printf("path: %s\n", file);
	}

	ret = checkMd5Value(file);
	if(ret==0){
		printf("file matched %s\n", file);
	}

	return 0;
}





