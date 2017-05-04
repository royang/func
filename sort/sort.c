#include <stdio.h>
#include <stdint.h>

unsigned short test[20] = {20,10,15,13,14,12,11,15,14,10,18,5,3,4,1,599};

void misc_get_max_values(unsigned short *samples, unsigned short len, unsigned short *max_values_index)
{
	unsigned short i=0;
	unsigned short samples_max_value[3]={0};
	
	for(i=0; i<len; i++)
	{
		if(samples[i] > samples_max_value[0])
		{
			samples_max_value[0] = 	samples[i];
			max_values_index[0] = i;
		}
		else if(samples[i] > samples_max_value[1])
		{
			samples_max_value[1] = 	samples[i];
			max_values_index[1] = i;
		}
		else if(samples[i] > samples_max_value[2])
		{
			samples_max_value[2] = 	samples[i];
			max_values_index[2] = i;
		}
	}
}

void main()
{
	unsigned short index[3] = {0};
	
	misc_get_max_values(test, 16, index);
	
	printf("get max values :\n");
	printf("%d : %d\n", index[0], test[index[0]] );
	printf("%d : %d\n", index[1], test[index[1]] );
	printf("%d : %d\n", index[2], test[index[2]] );
	
}