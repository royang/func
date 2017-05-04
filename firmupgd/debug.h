#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <stdio.h>

#define FREE(ptr) if(ptr){free(ptr); ptr = NULL;}

//chenyqf add
#ifdef LOG_TAG
#define LOGD(fmt...)   	\
	    do {			\
	        printf("[D/%s] %s,line:%d: ", LOG_TAG, __FUNCTION__, __LINE__);\
	        printf(fmt);\
	        printf("\n");\
	    }while(0);	
		
#define LOGE(fmt...)   	\
	    do {			\
	        printf("[E/%s] %s,line:%d: ", LOG_TAG, __FUNCTION__, __LINE__);\
	        printf(fmt);\
	        printf("\n");\
	    }while(0);			
#else
#define LOGD(fmt...)   	
#define LOGE(fmt...)
#endif


#define CHK(exp){if(exp < 0){printf("%s:%d:%s():  error happy \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

#define CHK_NULL(exp){if(exp == 0){printf("%s:%d:%s():  is NULL \
\n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

#define SW16(X)	((((unsigned short)(X) & 0xff00) >> 8) |(((unsigned short)(X) & 0x00ff) << 8))
#define SW32(X)	((((unsigned long)(X) & 0xff000000) >> 24) | (((unsigned long)(X) & 0x00ff0000) >> 8) | \
	(((unsigned long)(X) & 0x0000ff00) << 8) | (((unsigned long)(X) & 0x000000ff) << 24))

#define rMillion 1000000
	
#define TimeDifference( left, right ) (left.tv_sec  - right.tv_sec) +   \
			(left.tv_usec - right.tv_usec) / ((double) rMillion)

#endif
