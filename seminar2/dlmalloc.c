#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>


#define TRUE 1
#define FALSE 0
#define TAKENSTRUCT 1

#ifdef TAKENSTRUCT

/* TAKENSTRUCT = 1 */
#define HEAD (sizeof(struct taken))
#define MIN(size) (((size) > (16)?(size):(16)))
#define MAGIC(memory) ((struct taken*)memory - 1)
#define HIDE(block) (void*)((struct taken*)block + 1)

#else

/* TAKENSTRUCT = 0 */
#define HEAD (sizeof(struct head))	// changes as we scale head structure
#define MIN(size) (((size) > (8))?(size):(8)) // minimum block size containing data is 32 - 24 = 8 bytes. 
#define MAGIC(memory) ((struct head*)memory - 1)	// retrieving pointer to header memory address
#define HIDE(block) (void*)((struct head*)block + 1) // hiding header

#endif

#define LIMIT(size) (MIN(0) + HEAD + size) // size a block has to be larger than in order to split it
#define ALIGN 8 		// aligning by 8 bytes (64-bit architecture)
#define ARENA (64*1024)	// large block allocated

/* (8 bytes) header structure of block => smallest block 24 bytes (8 header + 16 data)*/
struct taken {
	uint16_t bfree;	// (2 bytes) status of block before
	uint16_t bsize;	// (2 bytes) size of block before
	uint16_t free;		// (2 bytes) status of the block
	uint16_t size;		// (2 bytes) size of block (max 2¹⁶b = 64 Kb)
};


/* (24 bytes) header structure of block => smallest block 32 bytes (24 header + 8 data) */
struct head {
	uint16_t bfree;	// (2 bytes) status of block before
	uint16_t bsize;	// (2 bytes) size of block before
	uint16_t free;		// (2 bytes) status of the block
	uint16_t size;		// (2 bytes) size of block (max 2¹⁶b = 64 Kb)
	struct head *next;	// (8 bytes) pointer to next
	struct head *prev;	// (8 bytes) pointer to previous	
};

/* returns memory address to block after */
struct head *after(struct head *block){
	return (struct head*)((char*) block + block->size + HEAD);
}

/* returns memory address to block before */
struct head *before(struct head *block){
	return (struct head*)((char*) block - block->bsize - HEAD);
}

/* splits one big block to form new block with >size< and returns pointer to it (second) */
struct head *split(struct head *block, int size) {
	//printf("split started\n");
	/* calculate remaining size */
	int rsize = block->size - size - HEAD;
	block->size = rsize; // shrinking original block size, since we will create a new directly after
	
	/* "second block" */
	struct head *splt = after(block); // initialize new block
	/* modifying attributes of block */
	splt->bfree = block->free;
	splt->bsize = block->size;
	splt->free = FALSE;
	splt->size = size; // user told us what the size should be
	
	/* during this part of the code I had some trouble understanding what the difference between the blocks were*/
	
	/* block is the remaining (shrinked) part of the original block */
	struct head *aft = after(splt);
	aft->bsize = splt->size;

	/* returns second block */ 
	return splt;
}

/* we set to null since we only have one arena */
struct head *arena = NULL;

struct head *new(){
	//printf("new started\n");
	if(arena != NULL){
		//printf ("one arena already allocated \n");
		return NULL;
	}
	
	/* using mmap, but could have used sbrk */
	/* creating new block (mapping) with as large virtual area as possible */
	// (void *addr, size_t len, int prot, int flags, int fildes, off_t off)
	/* since address argument is null, kernel chooses address to map */
	struct head *new = mmap(NULL, ARENA, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
	
	/* MAP_FAILED = (void *) -1 */
	if (new == MAP_FAILED){
		//printf("mmap failed: error %d \n", errno);
		return NULL;
	}
	
	/* payload, make room for head and sentinel */
	unsigned int size = ARENA - (2 * HEAD);
	
	new->bfree = FALSE; // so we don't merge with non-existing block before
	new->bsize = 0;
	new->free = TRUE;
	new->size = size; 
	
	/* initate footer directly after the first payload block in arena */
	struct head *sentinel = after(new);
	
	/* only touch the status fields */
	/* sentinel is zero element block in the end */
	/* allows us to set the footer and determine that we should not try to merge with it */
	sentinel->bfree = new->free;
	sentinel->bsize = size;
	sentinel->free = FALSE; // prevent anyone from trying to merge the block with the sentinel
	sentinel->size = 0;
	
	/* this is the only arena we have */
	arena = (struct head*)new;
	return new;
}



/* FREE LIST
 * Double linked list used to keep track of blocks in memory that are free.
 *
 */

struct head *flist;

/* detach a block from the free list */
void detach(struct head *block){
	//printf("detach started\n");
	/* if block to be removed is NOT in the absolute back of flist */
	/* is there any block after? */
	if(block->next != NULL){
		block->next->prev = block->prev;
	}
	
	/* if block to be removed is NOT in the absolute front of flist */
	/* is there any block before? */
	if(block->prev != NULL){
		block->prev->next = block->next;
	} else {
		flist = block->next;
	}
}

/* insert a new block in the free list */
void insert(struct head *block){
	//printf("insert started\n");
	/* make block first in flist, by making block-> next pointing at previous flist (old first)  */
	block->next = flist;
	block->prev = NULL;
	
	/* as long as flist is NOT empty, insert block first 
	and make sure flist (old first) points to the new block */
	if(flist != NULL)
		flist->prev = block;
		
	/* let poiter *flist point towards the new block that is now first in list */
	/* I first thought that flist was the whole list, 
	but later understood that it points to "first" block in list */
	flist = block; 
}


/* MALLOC & FREE
 * Allocate and free memory.
 *
 */

/* Determine suitable size and even multiple of ALIGN and not smaller than MIN */
int adjust(size_t size){
	//printf("->adjust started\n");
	int minsize = MIN(size);
	
	/* check size (if < 8, update to 8) and check if aligned with 8 bits */
	if(minsize % ALIGN == 0){
		//printf("->adjust returning - no need to align\n");
		return size;
	} else {
		/* calculate remainder */
		int adj = minsize % ALIGN;
		
		/* adjust(31) => return (31 + (8-7))     = 32 which is aligned */ 
		//printf("->adjust returning - aligned size\n");
		return (minsize + ALIGN - adj);
	}
}

/* used to find a block of right size */
/* Can look like: 
	1. traverse freelist to find suitable block to see if we find size <= block->size
	2. If no block is found, then create new block and add to freelist
*/
struct head *find(size_t size){
	//printf("->find started\n");
	/*if(size == 0){
		return NULL;
	}*/
	
	/* create duplicate copy of flist that we can modify and play around with without destroying */
	struct head *temp = flist;
	
	
	
	
	while(temp != NULL){
		if(temp->size >= size) {
			// suitable block found
			detach(temp); 
		
			/* compare if size is large enough to split */
			if(temp->size >= LIMIT(size)){
				// splitting block
				struct head *block = split(temp,size);
				insert(temp);
				
				struct head *aft = after(block);
				aft->bfree = FALSE;
				block->free = FALSE;
				
				return block;
			} else {
				temp->free = FALSE;
				struct head *aft = after(temp);
				aft->bfree = FALSE;
				return temp;
			}
		} else {
			temp = temp->next; // increment for while loop to find suitable block
		}
	}

	//printf("->find returning\n");
	return NULL;
}

struct head *merge(struct head *block){
	//printf("merge started\n");
	struct head *aft = after(block);
	
	/* is the block before free? merge block and block->prev! */
	if(block->bfree == TRUE){
		/* unlink the block before */
		struct head *bef = before(block);
		detach(bef);
		
		/* calculate and set the total size of the merged blocks */
		bef->size = bef->size + block->size + HEAD;
		
		/* update the block after the merged blocks */
		aft->bsize = bef->size;
		
		/* continue with the merged block, pointing to the start of the new merged one */
		block = bef;
	}
	
	/* is the block after free? merge block and aft! */ 
	if(aft->free == TRUE){
		/* unlink the block */
		detach(aft);
		
		/* calculate and set the total size of merged blocks */
		block->size = block->size + aft->size + HEAD;
		
		/* update the block after the merged block */
		aft = after(block);
		aft->bsize = block->size;
	}
	//printf("->merge returning\n");
	return block;
}



/* malloc() function allocates >requested< bytes and returns a pointer to the allocated memory */
void *dalloc(size_t requested){
	//printf("dalloc started\n");
	/* null pointer satisifies a request for 0 byte */
	if(requested <= 0)
		return NULL;
	
	int size = adjust(requested); //send to adjust to determine size and mulitple of ALIGN
	struct taken *taken = (struct taken*)find(size); // traversing through freelist to find enough big block
	if(taken == NULL) //if NULL is returned to taken, no block was found in the freelist
		return NULL;
	else
		return HIDE(taken); // returning pointer to the payload (censoring header)
}

/* free() function frees the memory space pointed to by >memory< */
void dfree(void *memory){
	//printf("dfree started\n");
	if(memory != NULL){
		struct head *block = (struct head*) MAGIC(memory); //pointing to meta info in header
		
		/* merge call goes here */
		//block = merge(block);
		
		struct head *aft = after(block); // this is the block located directly after block
		
		block->free = TRUE; // the block is no longer taken
		aft->bfree = block->free; // the block after will now know that the previous block is no longer free
		
		/* inserting the freed block in the free list */
		insert(block);
	}
	return;
}





void traverseblocks(){
	struct head *temp = arena;
	
	while(temp->size != 0){
		printf("address: %p\t, free: %d\t, size: %d\t, bfree: %d\t, bsize: %d\t\n", temp, temp->free, temp->size, temp->bfree, temp->bsize);
		
		temp = after(temp);
	}
}

void sanity(){
	//printf("sanity started\n");
	/* Check so that all blocks in the freelist have correct previous pointers */
	struct head *sanity = flist;
	struct head *prev = sanity->prev;
	
	int size = sanity->size;
	
	while(sanity != NULL && sanity->size != 0){
		/* Check so that block in the freelist actually is free */
		if(sanity->free != TRUE){
			printf("NOT OK - found a block that is not free");
			exit(1);
		}
		
		/* Check so that block in the freelist actually is aligned */
		if(sanity->size % ALIGN != 0){
			printf("NOT OK - found a block that is not aligned");
			exit(1);
		}
		
		if(sanity->prev != prev){
			printf("NOT OK - found a block with incorrect prev");
			exit(1);
		}
		
		prev = sanity;
		sanity = sanity->next;
	}
	printf("sanity check complete without any abnormalities \n");
}




void init(){
	struct head *first = new();
	insert(first);
}

int freelistlength(){
	int i = 0;
	struct head *temp = flist;
	while(temp != NULL){
		i++;
		temp = temp->next;
	}
	return i;
}

void sizes(int*buffer, int max){
	struct head *next = flist;
	int i = 0;
	while((next != NULL) & (i < max)){
		buffer[i] = next->size;
		i++;
		next = next->next;
	}
}






