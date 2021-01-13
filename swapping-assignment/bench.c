#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "random.h"

#define SAMPLE 20

void clear_page_table(pte *pg_tbl, int pgs){
	for(int i = 0; i < pgs; i++){
		pg_tbl[i].present = 0;
	}
}

int simulate(int *seq, pte *tbl, int refs, int frms, int pgs){
	int hits = 0;
	int allocated = 0;
	
	int i = 0;
	
	for(i = 0; i < refs; i++){
		int next = seq[i];
		pte *entry = &tbl[next];
		
		if(entry->present == 1){
			hits++;
		} else {
			if(allocated < frms){
				allocated++;
				entry->present = 1;
			} else {
				pte *evict;
				do{
					// select by random
					int rnd = rand() % pgs;
				} while(evict->present != 1);
				
				evict->present = 0;
				entry->present = 1;
			}
		}
	}
	return hits;
}


int main(){
	printf("# This is a benchmark of random replacements\n");
	printf("# %d page references\n", refs);
	printf("# %d pages\n", pages);
	printf("#\n#\n#frames\tratio\n");
	
	// size of the memory in frames
	int frames;
	
	int incr = pages/SAMPLES;
	
	for(frames = incr; frames <= pages; frames += incr){
		clear_page_table(table, pages);
		
		int hits = simulate(sequence, table, refs, frames, pages);
		
		float ratio = (float)hits/refs;
		
		printf("%d\t%.2f\n", frames, ratio);
	}
	
}
