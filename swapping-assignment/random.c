#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "random.h"

#define HIGH 20
#define FREQ 80
#define PAGES 100



/*
void init(int *sequence, int refs, int pages){
	
	for(int i = 0; i < refs; i++){
		sequence[i] = rand() % pages;
	}
}
*/

void init(int *seq, int refs, int pages){
	int high = (int)(pages*((float)HIGH/100));
	
	for(int i = 0; i < refs; i++){
		if(rand() % 100 < FREQ){
			// frequent case
			seq[i] = rand() % high;
		} else {
			// less frequent case
			seq[i] = high + rand() % (pages - high);
		}
	}
}

int main(int argc, char *argv[]){
	int refs = 10;
	int pages = 100;
	
	// defining page table hold an array of page table entries
	pte table[PAGES];
	
	int *sequence = (int*)malloc(refs*sizeof(int));
	
	init(sequence, refs, pages);
	
	// experiment to show it works
	for(int i = 1; i < refs; i++){
		printf(" , %d", sequence[i]);
	}
	printf("\n");
	
	return 0;
}
