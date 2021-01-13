


#include <stdlib.h> 
#include <unistd.h>



struct chunk{
	int size;
	struct chunk *next;
};

struct chunk *flist = NULL;


/* 3.2 find a free block */
void *malloc(size_t size){
	if(size == 0){
		return NULL;
	}
	
	struct chunk *next = flist;
	struct chunk *prev = NULL;
	
	while(next != NULL){
		if(next->size >= size){
			if(prev != NULL){
				prev->next = next->next;
			} else {
				flist = next->next;
			}
			return (void*)(next + 1);
		} else {
			prev = next;
			next = next->next;
		}
	}
	
	// we now include the head of each memory frame
	void *memory = sbrk(size + sizeof(struct chunk));
	
	
	if(memory == (void*) - 1){
		// memory allocation using sbrk(...) failed and returned -1. We therefore return NULL.
		return NULL;
	} else {
		// "The malloc() function allocates size bytes and returns a pointer to the allocated memory"
		struct chunk *cnk = (struct chunk*)memory;
		cnk->size = size;
		return (void*)(cnk + 1);
	}
}

/* 3.1 You do not ow me your freedom */
void free(void *memory){
	if(memory != NULL){
		// jumping back one chunk position
		struct chunk *cnk = (struct chunk*)((struct chunk*)memory - 1);
		cnk->next = flist;
		flist = cnk;
	}
	return;
}
