#include <stdlib.h>
#include <ucontext.h>
#include <assert.h>
#include <stdio.h>
#include "green.h"

#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4096

static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, NULL , FALSE}; // global green thread pointing to main context
static green_t *running = &main_green; // initially the running thread
static void init() __attribute__((constructor));


/* Setting up support for queue */
struct Queue *create_queue(){
	
	struct Queue *queue = (struct Queue*)malloc(sizeof(struct Queue));
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
	
}

Queue *ready_queue = NULL;

/*
green_t *rdyq_head = NULL;
green_t *rdyq_tail = NULL;
int rdyq_size = 0;
*/
void init(){
	getcontext(&main_cntx);
	ready_queue = create_queue();
}

void enqueue(struct Queue *queue, void *thread){
	/* Queue empty */ 
	if(queue->tail == NULL){
		// Empty.. insert first.. 
		queue->head = thread;
		queue->tail = thread;
		queue->length += 1;
		return;
	} 
	
	queue->tail->next = thread;
	queue->tail = thread;
	queue->length += 1;
	
	
	/*
	if(rdyq_size == 0){
		printf("Ready queue empty. Inserting thread in queue.\n");
		rdyq_head = green;
		rdyq_tail = green;
		rdyq_size++;
	} else {
		printf("Ready queue consists of %d items. Inserting thread last in queue.\n", rdyq_size);
		green->next = rdyq_head;
		rdyq_head = green;
		rdyq_size++;
	}
	*/
	
}

// no args since it should take first in list..
struct green_t *dequeue(struct Queue *queue) { 
	/* Queue empty. Return NULL. */
	if(queue->head == NULL){
		return NULL;
	}
	
	green_t *thread = queue->head;
	queue->head = queue->head->next;
	
	if(queue->head == NULL){
		queue->tail == NULL;
	}
	
	queue->length -= 1; 
	
	return thread;
	
	/*
	if(rdyq_size == 0){
		printf("ERROR: Ready queue empty. Attemt to dequeue thread failed.");
	}
	if(rdyq_size == 1){
		dqed_thread = rdyq_tail;
		rdyq_head = NULL;
		rdyq_tail = NULL;
		rdyq_size = 0;
	} else {
		dqed_thread = rdyq_tail;
		rdyq_tail = NULL;		
		rdyq_size -= 1;
	}
	return dqed_thread;
	*/
}


/* green_thread() will start execution of real function and, when returning from call, terminate thread */
void green_thread(){
	green_t *this = running;
	
	void *result = (*this->fun)(this->arg);
	
	/* Place waiting (joining) thread in ready queue */
	// add suppport for then this == NULL
	if(this->join != NULL){
		enqueue(ready_queue, this->join);
	}
	
	
	/* Save result of execution */
	this->retval = result;
	
	/* We're a zombie */
	this->zombie = TRUE;	
	
	/* Copying the next thread (to be executed) in ready queue */
	//green_t *next = rdyq_tail;
	green_t *next = dequeue(ready_queue);
	
	/* Replacing currently running thread with next thread in ready queue */ 
	running = next;
	
	/* Setting context */
	setcontext(next->context);
}
 
/* green_create(thread, function, arguments) */ 
int green_create(green_t *new, void *(*fun)(void*), void *arg){
	
	/* Defining pointer to next context structure, if current structure returns */
	ucontext_t *cntx = (ucontext_t *)malloc(sizeof(ucontext_t));
	
	/* cntx currently active context */
	getcontext(cntx);
	
	void *stack = malloc(STACK_SIZE);
	
	/* The properties of the context */
	cntx->uc_stack.ss_sp = stack;
	cntx->uc_stack.ss_size = STACK_SIZE;
	
	/* Creating context, but not yet executed (done by set or swap) */
	makecontext(cntx, green_thread, 0);
	
	/* Attach context, function to execute and pointer to arguments to thread */
	new->context = cntx; 
	new->fun = fun;
	new->arg = arg;
	new->next = NULL;
	new->join = NULL;
	new->retval = NULL;
	new->zombie = FALSE;
	
	/* add new to the ready queue here */
	enqueue(ready_queue, new);
	
	return 0;
}

/* pthread_yield() causes the calling thread to relinquish (släppa) the CPU. The thread is placed at the end of the run queue and another thread is scheduled to run. */
int green_yield(){
	printf("- YIELDING -\n");
	
	/* Currently running process is copied */
	green_t *susp = running;
	
	/* Add susp last in ready queue (thread suspended) */
	enqueue(ready_queue, susp);
	
	/* Copying the next thread (to be executed) from ready queue */
	green_t *next = dequeue(ready_queue);
	
	/* Replacing currently running thread with next thread in ready queue */ 
	running = next;
	
	/* Swapping context. It will save current state in susp->context and continue execution from next->context. */
	swapcontext(susp->context, next->context);
	
	return 0;
}

/* pthread_join() function waits for the thread specified by argument to terminate. If that thread has already terminated, then pthread_join() returns immediately. */
int green_join(green_t *thread, void **res){
	/* If thread have not yet terminated... */
	if(!thread->zombie){
		printf("Thread NOT zombie\n");
		green_t *susp = running;
		
		/* Add as joining thread */
		thread->join = susp;
		
		/* Select next thread for execution */
		green_t *next = dequeue(ready_queue);
		running = next;
		swapcontext(susp->context, next->context);
	}
	
	/* Collect result */
	if(thread->retval != NULL){
		*res = thread->retval;
	}
	
	/* Free context */
	free(thread->context);
	
	return 0;
}

/*
oucp = Old User Context Pointer ???

_________________________________________________________________________________

getcontext()		getcontext(ucp)
-------------
Initializes the structure pointed at by ucp to the currently active context.


_________________________________________________________________________________

setcontext()		setcontext(ucp)
-------------
Restores (continues) the user context pointed at by ucp. Context obtained by getcontext().


_________________________________________________________________________________

swapcontext()		swapcontext(oucp, ucp)
-------------
Saves current context in structure pointed to pointed to by oucp and activates context pointed to by ucp.


_________________________________________________________________________________

makecontext()		makecontext(ucp, funct, argc)
-------------
Modifies the context pointed to by ucp. When this context is later activated (using setcontext or swapcontext) the funct is called with argc. When returning, successor (->next???????????) context is activated.



*/


