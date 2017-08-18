#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char path[256];
	int ret;

	bzero(path, sizeof(path));	
	if(getcwd(path, sizeof(path)) != NULL){
		printf("current path is %s\n", path);
	}
	exit(EXIT_SUCCESS);
}
