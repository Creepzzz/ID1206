#include <stdlib.h>
#include <ucontext.h>
#include <assert.h>
#include <stdio.h>
#include "green.h"
#include <signal.h>
#include <sys/time.h>

#define FALSE 0
#define TRUE 1
#define STACK_SIZE 4096
#define PERIOD 100

static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, NULL , FALSE}; // global green thread pointing to main context
static green_t *running = &main_green; // initially the running thread
static void init() __attribute__((constructor));

static sigset_t block;
void timer_handler(int);
//test

/* Setting up support for queue */
struct Queue *create_queue(){
	
	struct Queue *queue = (struct Queue*)malloc(sizeof(struct Queue));
	queue->head = NULL;
	queue->tail = NULL;
	queue->length = 0;
	return queue;
	
}

Queue *ready_queue = NULL;


void init(){
	getcontext(&main_cntx);
	ready_queue = create_queue();
	
	/* Timer functionality */
	sigemptyset(&block);
	sigaddset(&block, SIGVTALRM);
	
	struct sigaction act = {0};
	struct timeval interval;
	struct itimerval period;
	
	act.sa_handler = timer_handler;
	assert(sigaction(SIGVTALRM, &act, NULL) == 0);
	
	interval.tv_sec = 0;
	interval.tv_usec = PERIOD;
	period.it_interval = interval;
	period.it_value = interval;
	setitimer(ITIMER_VIRTUAL, &period, NULL);
}

	

void enqueue(struct Queue *queue, void *thread){
	if(queue->tail == NULL){
		// Empty.. insert first.. 
		queue->head = thread;
		queue->tail = thread;
		queue->length += 1;
		
		// COND SIGNAL???
		return;
	} 
	
	queue->tail->next = thread;
	queue->tail = thread;
	queue->length += 1;
	//printf("queue length: %d\n", queue->length); 
	
}



// Dequeueing head/front/start
struct green_t *dequeue(struct Queue *queue) { 

	if(queue->head == NULL){
		return NULL;
	}
	
	green_t *thread = queue->head;
	queue->head = queue->head->next;
	
	if(queue->head == NULL){
		queue->tail == NULL;
	}
	
	queue->length -= 1; 
	//printf("queue length: %d\n", queue->length);
	
	return thread; 
}


/* green_thread() will start execution of real function and, when returning from call, terminate thread by setting zombie to true, and switching thread/context to run. */
void green_thread(){
	green_t *this = running;
	assert(this->fun != NULL);
	
	/* Running "real" function */
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
 
/* Initializes a green thread */
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

/* Suspends current thread and selects a new thread for execution */
/* pthread_yield() causes the calling thread to relinquish (släppa) the CPU. The thread is placed at the end of the run queue and another thread is scheduled to run. */
int green_yield(){
	//printf("- YIELDING -\n");
	
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

/* Suspends current thread waiting for a thread to terminate */
/* pthread_join() function waits for the thread specified by argument to terminate. If that thread has already terminated, then pthread_join() returns immediately. */
int green_join(green_t *thread, void **res){
	/* If thread have not yet terminated... make sure it is allowed to run again */
	if(!thread->zombie){
		//printf("Thread NOT zombie\n");
		green_t *susp = running;
		assert(susp != NULL);
		
		/* Add as joining thread */
		thread->join = susp;
		
		/* Select next thread for execution */
		green_t *next = dequeue(ready_queue);
		assert(next != NULL);
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








/******************************************
 *                                        *
 *           3. CONDITIONALS              *
 *                                        *
 ******************************************/

/* Initialize a green condition variable */
/* condition variable = used to wait for a condition to become true. Implemented as an explicit queue that threads can put themselves on to wait when some state is not as desired */
void green_cond_init(green_cond_t* cond){
	cond->susp_queue = create_queue();
}

/* This function is executed when a thread wishes to put it self to sleep. Suspends the current thread on the condition. The responsibility of wait() is to release the lock and put the calling thread to sleep (atomic)............ */
/* Release lock, put thread to sleep until cond is signaled. When thread wakes up again: re-aquire lock before returning. */
void green_cond_wait(green_cond_t* cond){
	/* BLOCK THE TIMER INTERRUPT */
	sigprocmask(SIG_BLOCK, &block, NULL);
	
	/* Currently running process is copied */
	green_t *susp = running;
	assert(susp != NULL);
	
	/* Add susp thread to the list (condition) */
	enqueue(cond->susp_queue, susp);
	
	/* Copying the next thread (to be executed) from ready queue */
	green_t *next = dequeue(ready_queue);
	assert(next != NULL);
	
	/* Replacing currently running thread with next thread in ready queue */ 
	running = next;
	
	/* Swapping context. It will save current state in susp->context and continue execution from next->context. */
	swapcontext(susp->context, next->context);
	
	/* UNBLOCK THE TIMER INTERRUPT */
	sigprocmask(SIG_UNBLOCK, &block, NULL);
}
/* Some other waiting thread can be woken up to be allowed to execute  */
/* This function is executed when a thread has changed something in the program and wants to wake sleeping thread waiting on its condition. */
/* If any threads are waiting on cond, wake up one of them. Caller must hold lock which must be the same as the lock used in the wait call. */
void green_cond_signal(green_cond_t* cond){
	/*if(cond->susp_queue->length == 0){
		assert(cond->susp_queue == NULL);
		return;
	}
	assert(cond->susp_queue != NULL && cond->susp_queue->length > 0); */
	
	green_t *thread = dequeue(cond->susp_queue);
	assert(thread != NULL);
	if(thread != NULL){
		enqueue(ready_queue, thread);
	}
}




/******************************************
 *                                        *
 *           4. TIMER INTERRUPT           *
 *                                        *
 ******************************************/

void timer_handler(int sig){
	/* BLOCK THE TIMER INTERRUPT */
	sigprocmask(SIG_BLOCK, &block, NULL);
	
	green_t *susp = running;
	assert(susp != NULL);
	
	/* Add the running to the ready queue */
	enqueue(ready_queue, susp);
	
	/* Find the next thread for execution */
	green_t *next = dequeue(ready_queue);
	assert(next != NULL);
	
	running = next;
	swapcontext(susp->context, next->context);
	
	/* UNBLOCK THE TIMER INTERRUPT */
	sigprocmask(SIG_UNBLOCK, &block, NULL);
}






/******************************************
 *                                        *
 *           5. MUTEX                     *
 *                                        *
 ******************************************/
 
 int green_mutex_init(green_mutex_t *mutex){
 	mutex->taken = FALSE;
 	mutex->mutex_queue = create_queue();
 }
 
 
 int green_mutex_lock(green_mutex_t *mutex){
 	/* BLOCK THE TIMER INTERRUPT */
	sigprocmask(SIG_BLOCK, &block, NULL);
 	
 	if(mutex->taken){
 		/* Suspend the running thread */
 		green_t *susp = running;
 		enqueue(mutex->mutex_queue, susp);
 		
 		/* Find next thread */
 		green_t *next = dequeue(ready_queue);
 		assert(next != NULL);
 		
 		running = next;
 		swapcontext(susp->context, next->context);
 	} else {
 		/* Take lock */
 		mutex->taken = TRUE;
 	}
 	
 	/* UNBLOCK THE TIMER INTERRUPT */
	sigprocmask(SIG_UNBLOCK, &block, NULL); 
	
	return 0;
 }
 
 int green_mutex_unlock(green_mutex_t *mutex){
 	/* BLOCK THE TIMER INTERRUPT */
	sigprocmask(SIG_BLOCK, &block, NULL);
 	
 	if(mutex->mutex_queue != NULL){
 		/* Move susp thread to ready queue */
 		green_t *susp = dequeue(mutex->mutex_queue);
 		enqueue(ready_queue, susp);
 		
 	} else {
 		/* Release lock */
 		mutex->taken = FALSE;
 	}
 	
 	/* UNBLOCK THE TIMER INTERRUPT */
	sigprocmask(SIG_UNBLOCK, &block, NULL);
 	
 	return 0;
 }
 
 

