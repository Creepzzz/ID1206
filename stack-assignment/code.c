#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

char global[] = "This is a global string";
const int read_only = 123456;

/* 1.1 The memory map
int main(){
	//label
	foo:
	
	//print conversion of label to value
	printf("the code: %p\n", &&foo);
	
	fgetc(stdin);
	
	return 0;
}
*/






/* 1.2 Code and read only data
int main(){
	int pid = getpid();
	
foo:
	
	printf("process id: \t%d\n", pid);
	printf("global string: \t%p\n", &global);
	printf("read only: \t%p\n", &read_only);
	printf("the code: \t%p\n", &&foo);
	
	printf("\n\n /proc/%d/maps \n\n", pid);
	char command[50];
	sprintf(command, "cat /proc/%d/maps", pid);
	
	system(command);
	
	return 0;
}
*/

/* PRINTOUT: 

	process id: 	3350
	global string:  0x56427d7d9010
	read only: 	0x56427d7d7004
	the code: 	0x56427d7d61ec

*/
	
	
	
	
	
	
	
	
/* 2.2 Back to stack 
int main(){
	int pid = getpid();
	
	// local variable assigned to the procedure only (stack)
	unsigned long p = 0x1;
	
	printf("     p  (0x%lx): %p\n", p, &p);
	
	printf("\n\n /proc/%d/maps \n\n", pid);
	char command[50];
	sprintf(command, "cat /proc/%d/maps", pid);
	
	system(command);
	
	return 0;
}
*/

/* PRINTOUT:
	     p  (0x1): 0x7fff7840e188

	.......

	7fff783ef000-7fff78410000 rw-p 00000000 00:00 0         [stack]

*/







/* 2.3 Pushing things to the stack */
// handles the printing
void zot(unsigned long *stop){
	unsigned long r = 0x3;
	
	unsigned long *i;
	for(i = &r; i <= stop; i++){
		printf("%p         0x%lx\n", i, *i);
	}
}

//used to allocate another stack frame
void foo(unsigned long *stop){
	unsigned long q = 0x2;
	zot(stop);
}

int main(){
	int pid = getpid();
	
	// local variable assigned to the procedure only (stack)
	unsigned long p = 0x1;
	
	foo(&p);
	
back: 
	printf("     p  (0x%lx): %p\n", p, &p);
	printf("     back: %p\n", &&back);
	
	printf("\n\n /proc/%d/maps \n\n", pid);
	char command[50];
	sprintf(command, "cat /proc/%d/maps", pid);
	
	system(command);
	
	return 0;
}

/* PRINTOUT: 

	0x7fffd59b9268         0x3			zot: r
	0x7fffd59b9270         0x7fffd59b9270		zot: i pointing to itself
	0x7fffd59b9278         0x10e2b93700f48f00
	0x7fffd59b9280         0x7fffd59b92b0
	0x7fffd59b9288         0x560513e09263
	0x7fffd59b9290         0x0
	0x7fffd59b9298         0x7fffd59b92c8
	0x7fffd59b92a0         0x0			
	0x7fffd59b92a8         0x2			foo: q
	0x7fffd59b92b0         0x7fffd59b9310
	0x7fffd59b92b8         0x560513e0929d
	0x7fffd59b92c0         0xe6b000000c2
	0x7fffd59b92c8         0x1			main: p
	     p  (0x1): 0x7fffd59b92c8
	     back: 0x560513e0929d


*/

