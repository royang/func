#include <stdio.h>
#include <stdlib.h>

struct record{
	char name[12];
	int age;
};

int main(int argc, char *argv[])
{
	struct record array[2];
	FILE *fp = fopen("recfile", "r");
	if(fp == NULL){
		perror("open file");
		exit(1);
	}

	fread(array, sizeof(struct record), 2, fp);
	printf("read name:%s, age:%d\n", array[0].name, array[0].age);
	printf("read name:%s, age:%d\n", array[1].name, array[1].age);
	
	return 0;
}
