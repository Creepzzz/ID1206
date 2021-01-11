#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/* 3 The Heap

int main(){
	int pid = getpid();
	
	// heap variable stored at the stack, pointing to the heap
	char *heap = malloc(20);
	
	printf("the heap variable at: %p\n", &heap);
	printf("pointing to: %p\n", heap);
	
	printf("\n\n /proc/%d/maps \n\n", pid);
	char command[50];
	sprintf(command, "cat /proc/%d/maps", pid);
	
	system(command);
	
	return 0;
}

*/

/* PRINTOUT: 

	the heap variable at: 0x7ffcaf1b7f48
	pointing to: 0x55fed43d82a0
	
*/






/* 3.1 free and reuse 

int main(){
	char *heap = malloc(20);
	*heap = 0x61;
	printf("heap pointing to: 0x%x\n", *heap);
	
	free(heap);
	
	char *foo = malloc(20);
	*foo = 0x62;
	printf("foo pointing to: 0x%x\n", *foo);
	
	// danger ahead 
	*heap = 0x63;
	printf("or is it pointing to: 0x%x\n", *foo);
	
	return 0;
}
*/

/* PRINTOUT: 
	heap pointing to: 0x61
	foo pointing to: 0x62
	or is it pointing to: 0x63
	
*/


	
/* 3.2 calloc */

int main(){
	int pid = getpid();
	
	long *heap = (unsigned long*)calloc(80, sizeof(unsigned long));
	
	printf("the heap variable at: %p\n", &heap);
	printf("pointing to: %p\n", heap);
	
	printf("heap[2]: 0x%lx\n", heap[2]);
	printf("heap[1]: 0x%lx\n", heap[1]);
	printf("heap[0]: 0x%lx\n", heap[0]);
	printf("heap[-1]: 0x%lx\n", heap[-1]);
	printf("heap[-2]: 0x%lx\n\n", heap[-2]);
	
	free(heap);
	
	printf("heap[2]: 0x%lx\n", heap[2]);
	printf("heap[1]: 0x%lx\n", heap[1]);
	printf("heap[0]: 0x%lx\n", heap[0]);
	printf("heap[-1]: 0x%lx\n", heap[-1]);
	printf("heap[-2]: 0x%lx\n\n", heap[-2]);
	
	printf("\n\n /proc/%d/maps \n\n", pid);
	char command[50];
	sprintf(command, "cat /proc/%d/maps", pid);
	
	system(command);
	
	return 0;
}
	

/* PRINTOUT: 

Case 1: calloc(40, ..)
--------------------------

	heap[2]: 0x0
	heap[1]: 0x0
	heap[0]: 0x0
	heap[-1]: 0x151
	heap[-2]: 0x0
	
	heap[2]: 0x0
	heap[1]: 0x5563f05a0010
	heap[0]: 0x0
	heap[-1]: 0x151
	heap[-2]: 0x0
	
	
Case 2: calloc(80, ..)
--------------------------
	heap[2]: 0x0
	heap[1]: 0x0
	heap[0]: 0x0
	heap[-1]: 0x291
	heap[-2]: 0x0
	
	heap[2]: 0x0
	heap[1]: 0x5563f05a0010
	heap[0]: 0x0
	heap[-1]: 0x291
	heap[-2]: 0x0

*/




