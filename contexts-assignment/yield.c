#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

#define MAX 10

static int running; // to keep track of which thread that is currently running
static int done1;
static int done2;

static ucontext_t cntx_one;
static ucontext_t cntx_two;
static ucontext_t cntx_main;
static ucontext_t cntx_done;

void yield(){
	printf(" - yield -\n");
	if(running == 1){
		printf("Switching (one, two)\n");		
		running = 2;
		swapcontext(&cntx_one, &cntx_two);
	} else {
		printf("Switching (two, one)\n");
		running = 1; // running was 2, so we set it to 1
		swapcontext(&cntx_two, &cntx_one);
	}
	
}

void push(int p, int i){
	if(i < MAX){
		printf("%d%*s push\n", p, i, " ");
		push(p, i+1);
		printf("%d%*s pop\n", p, i, " ");
	} else {
		printf("%d%*s top\n", p, i, " ");
		yield();
	}
}

void done() {
	int done = 0;
	
	while(!done){
		if(running == 1){
			printf(" - process one terminating -\n");
			done1 = 1;
			if(!done2){
			running = 2;
			swapcontext(&cntx_done, &cntx_two);
			} else {
				done = 1;
			}
		} else {
			printf(" - process two terminating -\n");
			done2 = 1;
			if(!done1){
				running = 1;
				swapcontext(&cntx_done, &cntx_one);
			} else {
				done = 1;
			}
		}
	}
	printf(" - done terminating -\n");
}

int main(){
	char stack1[8*1024];
	char stack2[8*1024];
	char stack_done[8*1024];
	
	/* The first context being initiatlized. */
	
	getcontext(&cntx_one);
	cntx_one.uc_link = &cntx_done;
	cntx_one.uc_stack.ss_sp = stack1;
	cntx_one.uc_stack.ss_size = sizeof(stack1);
	makecontext(&cntx_one, (void (*) (void)) push, 2, 1, 1);
	
	/* The second context being initiatlized. */
	
	getcontext(&cntx_two);
	cntx_two.uc_link = &cntx_done;
	cntx_two.uc_stack.ss_sp = stack2;
	cntx_two.uc_stack.ss_size = sizeof(stack2);
	makecontext(&cntx_two, (void (*) (void)) push, 2, 2, 1);
	
	/* The done context being initialized. */
	
	getcontext(&cntx_done);
	cntx_done.uc_link = &cntx_main;
	cntx_done.uc_stack.ss_sp = stack_done;
	cntx_done.uc_stack.ss_size = sizeof(stack_done);
	makecontext(&cntx_done, (void (*) (void)) done, 0);
	
	running = 1;
	
	printf(" - let's go! -\n"); 		// start
	swapcontext(&cntx_main, &cntx_one);	// everything happens here
	printf(" - that's all folks -\n");	// end
}
