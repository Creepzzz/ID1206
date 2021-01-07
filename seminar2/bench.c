#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "rand.h"
#include "dlmalloc.h"

#define ROUNDS 1000
#define LOOP 100000
#define BUFFER 100
#define SIZES 1000

int frequency[SIZES];

int cmp(const void *a, const void *b){
	return *(int*)a - *(int*)b;
}

/* during bench I encountered several segmentation faults after editing code, so I had to go back and re-do some basic functionallity */

void bench(int alloc, char *name){
	/* initiatlizing arena */
	init();

	void *buffer[BUFFER]; // buffer[100];
	for(int i = 0; i < BUFFER; i++){
		buffer[i] = NULL; // NULL all buffer positions
	}
	
	for(int i = 0; i < alloc; i++){
		int index = rand() % BUFFER;
		if(buffer[index] != NULL) {
			dfree(buffer[index]);
			buffer[index] = NULL;
		} else {
			size_t size = (size_t)request();
			int *memory;
			memory = dalloc(size);
			
			buffer[index] = memory;
			/* writing to the memory so we know it exists */
			*memory = 123;
		}
		
		FILE *file = fopen(name, "w");
		
		sizes(frequency, SIZES); // sizes(frequency, 100);
		int length = freelistlength();
		qsort(frequency, length, sizeof(int), cmp);
		
		for(int i = 0; i < length; i++){
			fprintf(file, "%d\n", frequency[i]);
		}
		
		fclose(file);
		
		printf("Length of the free list: %d\n", length);
	}
} 
/*
void benchimproved(){
	return 0;
}*/

/* argc = argument count     argv = argument vector */
int main(int argc, char *argv[]){
	if(argc < 3){
		printf("number of allocations as arg\n");
	}
	
	int first = atoi(argv[1]);
	char *name = argv[2];
	
	bench(first,name);
	//benchimproved(first);
	
	
	return 0;
}
