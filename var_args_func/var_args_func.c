#include <stdio.h>

void var_args_func(int a, double b, char *c)
{
	printf("a = %p\n", &a);
	printf("b = %p\n", &b);
	printf("&c = %p\n", &c);
	printf("c = %p\n", c);
}

int main()
{
//	printf("func = %p\n", &var_args_func);
//	var_args_func(2, 5.6, "hello");
	int i;
	int *p;

	printf("i = %d\n", i);
	printf("&i = %p\n", &i);

	printf("p = %p\n", p);
	printf("&p = %p\n", &p);

	i = 3;
	printf("\n\n i = 3\n");

	printf("i = %d\n", i);
	printf("&i = %p\n", &i);

	p = &i;
	printf("\n p = &i\n");
	printf("p = %p\n", p);
	printf("&p = %p\n", &p);
	printf("*p = %d\n", *p);

	*p = 5;
	printf("\n *p = 5\n");
	printf("*p = %d\n", *p);
	printf("i = %d\n", i);
	return 0;
}
