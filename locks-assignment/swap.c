#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

volatile int count = 0;

volatile int global = 0;

int try(volatile int *mutex){
	return __sync_val_compare_and_swap(mutex, 0, 1); // 0 = unlocked, 1 = locked
}

void lock(volatile int *mutex){
	while(try(mutex) != 0) {}; // spin
}

void unlock(volatile int *mutex){
	*mutex = 0;
}

typedef struct args {int inc; int id; volatile int *mutex;} args;

void *increment(void *arg){
	int inc = ((args*)arg)->inc; // increments the counter of thread
	int id = ((args*)arg)->id;     // identifier of thread
	volatile int *mutex = ((args*)arg)->mutex;
	
	printf("Starting thread %d\n", id);
	
	for(int i = 0; i < inc; i++){
		lock(mutex); // protect
		count ++; 
		unlock(mutex);
	}
}


int main(int argc, char *argv[]){
	if(argc != 2){
		printf("usage peterson <inc>\n");
		exit(0);
	}
	
	int inc = atoi(argv[1]);
	
	pthread_t one_p, two_p;
	args one_args, two_args;
	
	one_args.inc = inc;
	two_args.inc = inc;
	
	one_args.id = id;
	two_args.id = id;
	
	one_args.mutex = *mutex;
	two_args.mutex = *mutex;
	
	
	pthread_create(&one_p, NULL, increment, &one_args);
	pthread_create(&two_p, NULL, increment, &two_args);
	pthread_join(one_p, NULL); // await thread one to be finished
	pthread_join(two_p, NULL); // await thread two to be finished
	
	printf("result is %d \n", count);
	return 0;
}
