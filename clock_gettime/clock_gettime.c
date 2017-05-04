#include <stdio.h>
#include <time.h>

int main()
{
	int res;
	struct timespec time;

	while(1)
	{
		res = clock_gettime(CLOCK_REALTIME, &time);
		if(res)
		{
			printf("get time error\n");
		}

		printf("time: %d\n", time.tv_sec);
		sleep(1);
	}
}
