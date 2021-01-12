#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int volatile count;

/* 2. catching a signal 
// // impementing our own SIGINT handler that will do nothing useful
void handler (int sig){
	printf("signal %d ouch that hurt\n", sig);
	count++;
}

int main(){
	struct sigaction sa;
	
	int pid = getpid();

	printf("OK, lets go. Kill me (%d) if you can!\n", pid);

	//set the handler to be our function 
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	
	// change our signal table
	// when INT signal is sent we are doing whatever we want
	if(sigaction(SIGINT, &sa, NULL) != 0){
//		      ^       ^    ^ 
//		      |       |    └─ don't bother
//		      |       └─ pointer to sigaction structure
//		      └───── signal we want to handle
		return 1;
	}
	
	while(count != 4){
	}
	
	printf("I've had enough!\n");
	
	return 0;
}


*/

/* PRINTOUT: 

	OK, lets go. Kill me (18412) if you can!
	^Csignal 2 ouch that hurt
	^Csignal 2 ouch that hurt
	^Csignal 2 ouch that hurt
	^Csignal 2 ouch that hurt
	I've had enough!

*/









/* 3. Catch and throw 
// impementing our own SIGFPE handler that will kill a process
void handler(int sig){
	printf("signal %d was caught\n", sig);
	exit(1);
	return;
}

int not_so_good(){
	int x = 0;
	return 1 % x;
}

int main(){
	struct sigaction sa;
	
	int pid = getpid();

	printf("OK, I'll catch my own error..\n");
	
	//set the handler to be our function 
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	
	// change our signal table
	// now we catch FPE signals instead
	sigaction(SIGFPE, &sa, NULL);
//		    ^       ^    ^ 
//		    |       |    └─ don't bother
//		    |       └─ pointer to sigaction structure
//		    └───── signal we want to handle
	not_so_good();

	printf("Will probably not write this.....\n");
	
	return 0;
}
*/

/* PRINTOUT: 

	OK, I'll catch my own error..
	signal 8 was caught

*/










/* 4. who do you think you are 

int volatile done;

// impementing our own SIGFPE handler that will kill a process
void handler(int sig, siginfo_t *siginfo, void *context){
	printf("signal %d was caught\n", sig);
	
	// siginto_t structure contains information about the process that sent the signal
	printf("your UID is %d\n", siginfo->si_uid);
	printf("your PID is %d\n", siginfo->si_pid);
	
	done = 1;
}

int main(){
	struct sigaction sa;
	
	int pid = getpid();

	printf("OK, lets go. Kill me (%d) if you can!\n", pid);
	
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	
	if(sigaction(SIGINT, &sa, NULL) != 0){
//		      ^       ^    ^ 
//		      |       |    └─ don't bother
//		      |       └─ pointer to sigaction structure
//		      └───── signal we want to handle
		return 1;
	}
	
	while(!done){
	}
	
	printf("Told you so!!\n");
	
	return 0;
}
*/


/* PRINTOUT: 

shell 1: 
--------

	OK, lets go. Kill me (18586) if you can!
	^Csignal 2 was caught
	your UID is 0
	your PID is 0
	Told you so!!


shell 2: 
--------

	OK, lets go. Kill me (18586) if you can!
	signal 2 was caught
	your UID is 1000
	your PID is 18604
	Told you so!!

*/













/* 5. don't do this at home */

#define _GNU_SOURCE //to define REG_RIP

#include <stdio.h>
#include <signal.h>
#include <ucontext.h>

// cntx contains the context of the thread that caused the fault
static void handler(int sig_no, siginfo_t *info, void *cntx){
	ucontext_t *context = (ucontext_t*)cntx;
	unsigned long pc = context->uc_mcontext.gregs[REG_RIP];
	
	printf("Illegal instruction at 0x%lx value 0x%x\n", pc, *(int*)pc);
	context->uc_mcontext.gregs[REG_RIP] = pc + 1;
}

int main(){
	struct sigaction sa; 
	
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	
	sigaction(SIGSEGV, &sa, NULL);
	
	printf("Let's go!\n");
	
	// bad move. only for practicing purposes
	asm(".word 0x00000000");
	
here:
	printf("Piece of cake, this call is here %p\n", &&here);
	
	return 0;
}



/* PRINTOUT: 

	Let's go!
	Illegal instruction at 0x55677694f299 value 0xff30000
	Piece of cake, this call is here 0x55677694f29b


*/
