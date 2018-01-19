#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct record{
	char name[12];
	int age;
};


int main()
{
	struct record array[2]={{"ken", 28}, {"Kunth", 29}};
	FILE *fp = fopen("recfile", "w");
	if(fp == NULL){
		perror("open file");
		exit(1);
	}

	fwrite(array, sizeof(struct record), 2, fp);
	fclose(fp);
	return 0;
}
