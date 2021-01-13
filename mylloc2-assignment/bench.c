#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "rand.h"


/* 2. The interface

#define ROUNDS 1000
#define LOOP 100000

int main(){
	void *current;
	//pointer to top of heap
	void *init = sbrk(0);
	
	printf("The initial top of the heap is %p.\n", init);
	
	for(int j = 0; j < ROUNDS; j++){
		for(int i = 0; i < LOOP; i++){
			size_t size = (request() % 4000) + sizeof(int);
			int *memory;
			memory = malloc(size);
			if(memory == NULL){
				fprintf(stderr, "malloc failed\n");
				return 1;
			}
			//writing to memory so we know it exists
			//*memory = 123;
			free(memory);
		}
		current = sbrk(0);
		int allocated = (int)((current - init) / 1024);
		printf("%d\n", j);
		printf("The current top of heap is %p.\n", current);
		printf("    increased by %d Kbyte\n", allocated);
	}
	return 0;
}

*/

/* PRINTOUT: 

	The initial top of the heap is 0x55d66059b000.
	0
	The current top of heap is 0x55d66c4cdeaa.
	    increased by 195787 Kbyte
	1
	The current top of heap is 0x55d678469b39.
	    increased by 391994 Kbyte
	2
	The current top of heap is 0x55d68442e0b1.
	    increased by 588364 Kbyte
	3
	The current top of heap is 0x55d6903b2902.
	    increased by 784478 Kbyte
	: 
	: 

*/







/* 2.5 a buffer of blocks  */


#define ROUNDS 1000
#define LOOP 100000
#define BUFFER 100

int main(){

	// Initialize buffer
	void *buffer[BUFFER];
	// Empty buffer before using it 
	for(int i = 0; i < BUFFER; i++){
		buffer[i] = NULL;
	}
	
	void *current;
	//pointer to top of heap
	void *init = sbrk(0);
	
	printf("The initial top of the heap is %p.\n", init);
	
	for(int j = 0; j < ROUNDS; j++){
		for(int i = 0; i < LOOP; i++){
			int index = rand() % BUFFER;
			if(buffer[index] != NULL){
				// buffer position not empty, so we free it
				free(buffer[index]);
				buffer[index] = NULL;
			} else {
				// buffer position empty, so we allocate memory
				size_t size = (size_t)request();
				int *memory;
				memory = malloc(size);
				
				if(memory == NULL){
					fprintf(stderr,"memory allocation failed\n");
					return 1;
				}
				buffer[index] = memory;
				//writing to memory so we know it exists
				*memory = 123;
			}
		}
		current = sbrk(0);
		int allocated = (int)((current - init) / 1024);
		printf("%d\n", j);
		printf("The current top of heap is %p.\n", current);
		printf("    increased by %d Kbyte\n", allocated);
	}
	return 0;
}


/* PRINTOUT: 

Using mylloc.c as malloc() function
-------------------------------------

$ gcc -o bench mylloc.o rand.o bench.c -lm
$ strace ./bench 2>&1 > /dev/null | grep brk | wc -l

	882424			<----- times requested memory from kernel
	


Using mhysa.c as malloc() function
-------------------------------------

$ gcc -o bench mhysa.o rand.o bench.c -lm
$ strace ./bench 2>&1 > /dev/null | grep brk | wc -l

	184			<----- times requested memory from kernel



Using built in (default) malloc()
-------------------------------------

$ gcc -o bench rand.o bench.c -lm
$ strace ./bench 2>&1 > /dev/null | grep brk | wc -l

	11			<----- times requested memory from kernel
	


*/










