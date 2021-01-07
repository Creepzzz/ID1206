#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "rand.h"
#include "dlmalloc.h"

#define ROUNDS 100000
#define LOOP 1000
#define BLOCKS 1000

int main(){
	int *array[BLOCKS];
	
	init();
	clock_t start, stop;
	
	start = clock();
	
	for(int i = 0; i < BLOCKS; i++){
		array[i] = dalloc(16);
	}
	
	for(int i = 0; i < ROUNDS; i++){
		for(int j = 0; j < LOOP; j++){
			*array[j] = 100;
		}
	}
	stop = clock();
	printf("Time spent: %ld\n", (long)(stop - start));
}
