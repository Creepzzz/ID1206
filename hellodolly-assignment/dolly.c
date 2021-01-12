#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>



/* 2. The mother and the child 
int main(){
	int pid = fork();
	
	if(pid == 0){
		printf("I'm child %d. My knowledge of mom is %d.\n", getpid(), pid);
	} else {
		printf("I'm mom %d. My child is %d.\n", getpid(), pid);
	}
	
	printf("That's it %d!\n", getpid());
	
	return 0;
}

*/

/* PRINTOUT: 

	I'm mom 16888. My child is 16889.
	That's it 16888!
	I'm child 16889. My knowledge of mom is 0.           <---- this solved in ch. 3
	That's it 16889!


*/






/* 2.1 wait a minute 
int main(){
	int pid = fork();
	
	if(pid == 0){
		printf("\tI'm child %d. My knowledge of mom is %d.\n", getpid(), pid);
		sleep(1);
	} else {
		printf("I'm mom %d. My child is %d.\n", getpid(), pid);
		
		// wait for anything mother spawned (child) to terminate 
		wait(NULL);
		printf("My child has terminated.\n");
	}
	
	printf("That's it %d!\n", getpid());
	
	return 0;
}
*/

/* PRINTOUT: 

	I'm mom 16913. My child is 16914.
		I'm child 16914. My knowledge of mom is 0.
	That's it 16914!
	My child has terminated.
	That's it 16913!

*/







/* 2.2 returning a value
 int main(){
	int pid = fork();
	
	if(pid == 0){
		printf("child %d terminating here, not continuing after this.\n", getpid());
		return 42;
	} else {
		int res;
		//wait and place, whatever value the child returned, at &res
		wait(&res);
		printf("The child returned %d\n", WEXITSTATUS(res));
	}
	
	printf("That's it %d! (only mother terminates down here) \n", getpid());
	
	return 0;
}
*/


/* PRINTOUT: 

	child 16986 terminating here, not continuing after this.
	The child returned 42
	That's it 16985! (only mother terminates down here) 

*/






/* 2.4 a clone of the process 
int main(){
	
	int pid;
	int x = 123;
	printf("shared:\tx is %d and the virtual address is 0x%p\n", x, &x);
	
	pid = fork();
	
	if(pid == 0){
		printf("child:\tx = %d\n", x);
		x = 42;
		sleep(1);
		printf("child:\tx = %d\n", x);
		printf("child:\tx is %d and the virtual address is 0x%p\n", x, &x);
		
	} else {
		printf("mother:\tx = %d\n", x);
		x = 13;
		sleep(1);
		printf("mother:\tx = %d\n", x);
		printf("mother:\tx is %d and the virtual address is 0x%p\n", x, &x);
		wait(NULL);
	}
	
	return 0;
}
*/


/* PRINTOUT: 

	shared:	x is 123 and the virtual address is 0x0x7ffed80d7c00
	mother:	x = 123
	child:		x = 123
	mother:	x = 13
	mother:	x is 13 and the virtual address is 0x0x7ffed80d7c00
	child:		x = 42
	child:		x is 42 and the virtual address is 0x0x7ffed80d7c00


*/









/* 3. Groups, orphans, sessions and daemons 
int main(){
	
	int pid = fork();
	
	if(pid == 0){
		printf("child:\tI'm child %d and my mom is %d\n", getpid(), getppid());
		
	} else {
		
		printf("mom:\tI'm mom %d and my parent is %d\n", getpid(), getppid());
		wait(NULL);
	}
	
	return 0;
}
*/

/* PRINTOUT: 

	mom:	I'm mom 17492 and my parent is 3632
	child:	I'm child 17493 and my mom is 17492

$ echo $$
	3632		<------ process ID of current shell aka bash (parent to mom)
	
	// lets find parent of bash

$ ps -fp $$
	UID          PID    PPID  C STIME TTY          TIME CMD
	filip       3632    3621  0 04:08 pts/0    00:00:00 bash

$ ps -fp 3621
	UID          PID    PPID  C STIME TTY          TIME CMD
	filip       3621     692  0 04:08 ?        00:00:26 /usr/libexec/gnome-terminal-server
								^--- does process handling all gnome-terminal windows

$ ps -fp 692
	UID          PID    PPID  C STIME TTY          TIME CMD
	filip        692       1  0 01:53 ?        00:00:01 /lib/systemd/systemd --user

$ ps -fp 1
	UID          PID    PPID  C STIME TTY          TIME CMD
	root           1       0  0 01:52 ?        00:00:05 /sbin/init splash
	
$ ps -fp 0
	error: process ID out of range



*/









/* 3.1 The group */
int main(){
	
	int pid = fork();
	
	if(pid == 0){
		int child = getpid();
		printf("child:\tI'm child %d in group %d\n", child, getpgid(child));
		
	} else {
		int mom = getpid();
		printf("mom:\tI'm mom %d in group %d, which I'm the leader of\n", mom, getpgid(mom));
		wait(NULL);
	}
	
	return 0;
}



/* PRINTOUT:

	mom:	I'm mom 17723 in group 17723, which I'm the leader of
	child:	I'm child 17724 in group 17723

*/









