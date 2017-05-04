#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
	int pid = 0;

	pid = getpid();

	printf("process id : %d\n", pid);

	while(1)
	{
		sleep(1);
	}
}
