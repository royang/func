#include <stdio.h>
#include "MathFunc/sip_math.h"

int main(int argc, char *argv[])
{
	int a, b, c;

	if(argc < 3){
		printf("usage: %d numbers\n", argc);
		return 0;
	}

	a = atoi(argv[1]);
	b = atoi(argv[2]);
	c = m_plus(a, b);

	printf("%d + %d = %d\n", a, b, c);
	return 0;
}
