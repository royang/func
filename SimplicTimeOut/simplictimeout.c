#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define SIMPLIC_TIMEOUT_MAX_EVENTS	10
struct{
	void *(*cb)(void *arg);
	uint16_t time;
	uint32_t reg_time;
	void *arg;
}simplic_events[SIMPLIC_TIMEOUT_MAX_EVENTS];

uint32_t clock_s = 0;

int test = 16;
int SimplicTimeoutReg(uint8_t id, uint16_t time, void *(*cb)(void *arg), void *arg)
{
	simplic_events[id].cb = cb;
	simplic_events[id].time = time;
	simplic_events[id].reg_time = clock_s;
	simplic_events[id].arg = arg;
	printf("reg events id: %d, time %dms, reg time %d\n", id, simplic_events[id].time, simplic_events[id].reg_time);
} 

void SimplicTimeoutLogout(uint8_t id)
{
	simplic_events[id].time = 0;
	simplic_events[id].cb = NULL;
	printf("events %d logout\n", id);
}

void SimplicTimeServer(void)
{
	int i=0;

	for(i=0; i<SIMPLIC_TIMEOUT_MAX_EVENTS; i++){
		if((clock_s - simplic_events[i].reg_time > simplic_events[i].time)&&(simplic_events[i].time > 0)){
			printf("events %d timeout\n", i);
		
			simplic_events[i].cb(simplic_events[i].arg);
		}
	}
}

void *printhello(int *arg)
{
	static cnt = 0;

	printf("get %d\n",*arg);
	if(cnt < 5){
		SimplicTimeoutReg(1, 1500, &printhello, (void *)&test);
	}else{
		SimplicTimeoutLogout(1);
	}
	cnt++;
}

void *clockrun(void *arg)
{
	while(1){
		usleep(1000);
		clock_s++;
	}
}


int main()
{
	pthread_t pid;
	int ret;

	ret = pthread_create(&pid, NULL, &clockrun, (void *)NULL);
	if(ret < 0){
		perror("pthread_create");
		exit(-1);
	}
	
	SimplicTimeoutReg(1, 1500, &printhello, (void *)&test);
 	
	while(1){
		SimplicTimeServer();
	}
}


