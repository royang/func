#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *p=NULL;
	size_t size;

	p = malloc(100);
	if(!p){
		printf("not enough memory\n");
	}

	size = _msize(p);
	printf("get memory size %d\n", size);
	
	return 0;	
}
