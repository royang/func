
#include <stdio.h>


//chenyqf add
#ifdef LOG_TAG
#define ulog(fmt...)   	\
	    do {			\
	        printf("[D/%s] %s,line:%d: ", LOG_TAG, __FUNCTION__, __LINE__);\
	        printf(fmt);\
	        printf("\n");\
	    }while(0);	
#else
#define ulog(fmt...)   	\
	    do {			\
	        printf("[D/%s] %s,line:%d: ", "system", __FUNCTION__, __LINE__);\
	        printf(fmt);\
	        printf("\n");\
	    }while(0);	

#endif

