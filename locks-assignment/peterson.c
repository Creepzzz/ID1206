#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

volatile int count = 0;

volatile int request[2] = {0,0}; // structure to know if the threads are interested or not to access
volatile int turn = 0;

void lock(int id){
	request[id] = 1; // setting my flag to 1
	int other = 1-id;
	turn = other;
	while(request[other] == 1 && turn == other) {}; // spin
}

void unlock(int id){
	request[id] = 0;
}

typedef struct args {int inc; int id;} args; // groups arguments

void *increment(void *arg){
	int inc = ((args*)arg)->inc; // increments the counter of thread
	int id = ((args*)arg)->id;     // identifier of thread
	
	printf("Starting thread %d\n", id);
	
	for(int i = 0; i < inc; i++){
		lock(id); // protect
		count ++; 
		unlock(id);
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
	
	one_args.id = 0;
	two_args.id = 1;
	
	
	pthread_create(&one_p, NULL, increment, &one_args);
	pthread_create(&two_p, NULL, increment, &two_args);
	pthread_join(one_p, NULL); // await thread one to be finished
	pthread_join(two_p, NULL); // await thread two to be finished
	
	printf("result is %d \n", count);
	return 0;
}
