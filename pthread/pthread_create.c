#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


void *thr_fn1(void *arg){
	int i;

	for(i=0; i<5; i++){
		printf("thread 1 returning %d\n", i);
		sleep(1);
	}

	return((void *)1);
}

void *thr_fn2(void *arg){
	int i;

	for(i=0; i<5; i++){
		printf("thread 2 exiting %d\n", i);
		sleep(1);	
	}

	pthread_exit((void *)2);
}

int main(void)
{
	int err;
	pthread_t tid1, tid2;
	void *tret;

	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	if(err != 0){
		perror("thread 1 create");
		exit(-1);
	}

	err = pthread_join(tid1, &tret);
	if(err != 0){
		perror("thread 1 join");
		exit(-1);
	}else{
		printf("thread 1 exit code %ld\n", (long)tret);
	}

	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	if(err != 0){
		perror("thread 2 create");
		exit(-1);
	}

	err = pthread_join(tid2, &tret);
	if(err != 0){
		perror("thread 2 join");
		exit(-1);
	}else{
		printf("thread 2 exit code %ld\n", (long)tret);
	}
	
	return 0;
}
