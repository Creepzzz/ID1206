#include <stdio.h>
#include <stdlib.h>
#include "green.h"
#include <unistd.h>

int counter = 0;
int loop = 0;

green_mutex_t mutex;



void *test_timer(void *arg){
	for(int i = 0; i < loop; i++){
		green_mutex_lock(&mutex);
		counter++;
		green_mutex_unlock(&mutex);
	}
}



int main(int argc, char *argv[]){
	// take number from keyboard input
	loop = atoi(argv[1]);
	
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
