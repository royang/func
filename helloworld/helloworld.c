#include <stdio.h>

extern char **environ;

int main()
{
	char a[]="heartbeat";

	if(memcmp(a, "heartbeat", 9))
	{
		printf("n\n");
	}
	else
	{
		printf("y\n");
	}

	return 0;
}
