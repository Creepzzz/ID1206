#include <stdio.h>
#include <unistd.h>

int main(){
	int pid = fork();
	
	if(pid == 0){
		//child
		printf("Child: Here I am as well. I inherited mom's memory space.\n");
	} else {
		//mother
		printf("Mom: I'm here. I continue as usual. \n");
	}
	wait(NULL);
}
