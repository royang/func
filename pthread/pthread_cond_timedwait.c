#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

pthread_cond_t cond;
pthread_mutex_t mutex;
int flag = 1;

void *thr_fn(void *arg)
{
	struct timeval now;
	struct timespec outtime;
	int timeout_cnt = 0;

	pthread_mutex_lock(&mutex);

	while(flag){
		printf("wait timeout %d\n", timeout_cnt);
		gettimeofday(&now, NULL);
		outtime.tv_sec = now.tv_sec + 1;
		outtime.tv_nsec = now.tv_usec*1000;
		pthread_cond_timedwait(&cond, &mutex, &outtime);
		timeout_cnt++;
		if(timeout_cnt > 5){
			printf("wait condition timeout max\n");
			exit(-1);
		}
	}
	pthread_mutex_unlock(&mutex);
	printf("cond thread exit\n");
}

int main(void)
{
	pthread_t tid;
	char c;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	if(pthread_create(&tid, NULL, thr_fn, (void *)NULL)){
		perror("new thread create");
		return 1;
	}
	
	while((c = getchar()) != 'q');
	printf("teminate cond thread\n");

	pthread_mutex_lock(&mutex);
	flag = 0;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	printf("wait for thread to exit \n");
	pthread_join(tid, NULL);
	printf("cond thread exit\n");
	
	return 0;	
}

