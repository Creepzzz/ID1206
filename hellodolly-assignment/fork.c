#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	int pid = fork();
	
	if(pid == 0){
		//child
		printf("child: Here I am as well. I inherited mom's memory space.\n");
	} else {
		//mother
		printf("mom: I'm here. I continue as usual. \n");
	}
	wait(NULL);
	
	return 0;
}

/* PRINTOUT:

	mom: I'm here. I continue as usual. 
	child: Here I am as well. I inherited mom's memory space.
	
*/
