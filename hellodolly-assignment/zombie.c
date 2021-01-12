#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


/* 2.3 a zombie 
int main(){
	sleep(10);
	int pid = fork();
	
	if(pid == 0){
		printf("\nchild:\tcheck the status\n");
		sleep(10);
		printf("\nchild:\tand again \n");
		printf("child:\tnow terminating, becoming zombie\n");
		return 42;
	} else {
		sleep(20);
		printf("\nmom: (calling wait)\n");
		int res;
		// wait(...) corresponds to green_join() in seminar 3. If mom doens't issue call to wait in order to pick up return value
		wait(&res);
		printf("mom:\tthe result was %d\n", WEXITSTATUS(res));
		printf("mom:\tand again\n");
		sleep(10);
	}
	
	printf("\nThat's it %d!\n", getpid());
	
	return 0;
}
*/

/* PRINTOUT: 

	child:	check the status
	
	
		--------------------------------------------------------------
					    PID STAT COMMAND
					  	: 
		mother	>>>>>>		  17114 S    ./zombie
		child	>>>>>>		  17115 S    ./zombie
					  17116 R+   ps -ao pid,stat,command
					  	: 
		--------------------------------------------------------------


	child:	and again 
	child:	now terminating, becoming a zombie
	
	
		--------------------------------------------------------------
					    PID STAT COMMAND
				  		: 
		mother	>>>>>>		  17114 S    ./zombie
		child	>>>>>>		  17115 Z    [zombie] <defunct>
					  17117 R+   ps -ao pid,stat,command
					  	:
		--------------------------------------------------------------
	
	
	mom:	(calling wait)
	mom:	the result was 42
	mom:	and again
	
	
		--------------------------------------------------------------
					    PID STAT COMMAND
				  		: 
		mother	>>>>>>		  17114 S    ./zombie
					  17118 R+   ps -ao pid,stat,command
					  	:
	 	--------------------------------------------------------------


	That's it 17024!
	
	
		--------------------------------------------------------------
					    PID STAT COMMAND
				  		: 
					  17119 R+   ps -ao pid,stat,command
					  	:
		--------------------------------------------------------------

	
*/



