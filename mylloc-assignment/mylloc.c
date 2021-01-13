#include <stdlib.h> 
#include <unistd.h>

void *malloc(size_t size){
	if(size == 0){
		return NULL;
	}
	
	// initialize heap space and save pointer to later be returned
	void *memory = sbrk(size);
	
	
	if(memory == (void*)-1){
		// memory allocation using sbrk(...) failed and returned -1. We therefore return NULL.
		return NULL;
	} else {
		// "The malloc() function allocates size bytes and returns a pointer to the allocated memory"
		return memory;
	}
}

void free(void *memory){
	return;
}
