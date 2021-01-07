#include <stdio.h>
#include <stdlib.h>
#include "green.h"
#include <unistd.h>

green_cond_t cond;
green_mutex_t mutex;

int flag = 0;

void *test(void *arg){
	int id = *(int*)arg;
	int loop = 10000;
	while(loop > 0){
		green_mutex_lock(&mutex);
		printf("thread %d: %d\n", id, loop);
		while(flag != id){
			green_cond_wait(&cond, &mutex);
		}
		flag = (id + 1) % 2;
		green_cond_signal(&cond);
		green_mutex_unlock(&mutex);
		loop--;
	}
	return NULL;
}



int main(){
	
	green_t g0, g1;
	
	int a0 = 0;
	int a1 = 1;
	
	green_cond_init(&cond);
	green_mutex_init(&mutex);

	green_create(&g0, test, &a0);
	green_create(&g1, test, &a1);
	
	green_join(&g0, NULL);
	green_join(&g1, NULL);
	
	printf("Done with everything!\n");
	
	return 0;
}
