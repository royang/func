#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

int main()
{
	time_t value;

	while(1)
	{
		value = time(NULL);
		printf("%dl \n", value);
		sleep(1);
	}
	
}
