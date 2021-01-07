#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

int main(){
	int done = 0;
	ucontext_t one;
	ucontext_t two;
	
	
	/* Populate the context one with the state of the currently running context */
	getcontext(&one); // stores program counter, etc. into context data structure "one"
	
	printf("hello \n");
	
	if(!done){
		done = 1; // so we only switch once
		
		/* Saves the current execution context in structure two and copy context of one into proper registers of CPU */
		printf("We're now performing swapcontext(&two, &one) \n");
		swapcontext(&two, &one);
		printf("Swapping complete");
	}

	printf("Ending \n");
	
	return 0;
}
