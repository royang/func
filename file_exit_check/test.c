#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char md5[]={0x12, 0x34, 0x56};
	char str[7];
	int i;

	memset(str, '\0', sizeof(str));

	for(i=0; i<sizeof(md5); i++){	
		snprintf(str+i*2, 3, "%02x", md5[i]);
		printf("%02x ", md5[i]);
	}
	printf("\n%s\n", str);

	return 0;
}
