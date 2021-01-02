#include <stdio.h>
#include <stdlib.h>
#include "green.h"

int flag = 0;
green_cond_t cond;

void *test_timer(void *arg){
	int id = *(int*)arg;
	int loop = 4;
	while(loop > 0){
		if(flag == id){
			printf("thread %d: %d \n", id, loop);
			for(int i = 0; i < 1000000; i++){
				if(i < 1){
				i++;
				}
			}
			loop--;
			flag = (id + 1) % 2;
			green_cond_signal(&cond);
		} else {
			green_cond_signal(&cond);
			green_cond_wait(&cond);
		}
	}
}



int main(){
	green_t g0, g1, g2;
	
	int a0 = 0;
	int a1 = 1;
	int a2 = 2;
	
	green_cond_init(&cond);

	green_create(&g0, test_timer, &a0);
	green_create(&g1, test_timer, &a1);
	//green_create(&g2, test_cond, &a2);
	
	green_join(&g0, NULL);
	green_join(&g1, NULL);
	//green_join(&g2, NULL);
	
	printf("Done with everything!\n");
	
	return 0;
}
