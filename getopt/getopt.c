#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int opt;
	
//	opterr = 0;

	while( (opt = getopt(argc, argv, "nae:")) != -1){
		switch(opt){
				case 'n':printf("option = n, optopt = %c, optarg = %s\n", optopt, optarg);
					 break;
				case 'a':printf("option = a, optopt = %c, optarg = %s\n", optopt, optarg);
					 break;
				case 'e':printf("option = e, optopt = %c, optarg = %s\n", optopt, optarg);
					 break;
				default:printf("option = %c, optopt = %c, optarg = %s\n", opt, optopt, optarg);
					 break;
		}
		printf("argv[%d] = %s\n", optind, argv[optind]);
	}

	printf("opt = -1, optind = %d\n", optind);

	for(opt = optind; opt < argc; opt++){
		printf("---argv[%d]:%s\n", opt, argv[opt]);
	}

	for(opt = 1; opt < argc; opt++){
		printf("***argv[%d]:%s\n", opt, argv[opt]);
	}

	return 0;
}
