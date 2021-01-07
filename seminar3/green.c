#include <stdlib.h>
#include <ucontext.h>
#include <assert.h>
#include <stdio.h>
#include "green.h"
#include <signal.h>
#include <sys/time.h>
#include <sys/ucontext.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1
#define STACK_SIZE 4096
#define PERIOD 100

static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, NULL , FALSE};

static green_t *running = &main_green; // initially the running thread

struct green_t *ready_queue = NULL;

static void init() __attribute__((constructor));

static sigset_t block;
void timer_handler(int);

void init(){
	getcontext(&main_cntx);
	
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

	

green_t *dequeue(green_t **list){
	if(*list == NULL){
		return NULL;
	} else {
		green_t *thread = *list;
		*list = (*list)->next;
		thread->next = NULL;
		return thread;
	}
}

void enqueue(green_t **list, green_t *thread){
	if(*list == NULL){
		*list = thread;
	} else {
		green_t *susp = *list;
		while(susp->next != NULL){
			susp = susp->next;
		}
		susp->next = thread;
	}
}


/* green_thread() will start execution of real function and, when returning from call, terminate thread by setting zombie to true, and switching thread/context to run. */
void green_thread(){
	sigprocmask(SIG_BLOCK, &block, NULL);
	
	green_t *this = running;
	assert(this->fun != NULL);
	
	/* Running "real" function */
	void *result = (*this->fun)(this->arg);
	
	/* Place waiting (joining) thread in ready queue */
	if(this->join != NULL){
		enqueue(&ready_queue, this->join);
	}
	
	/* Save result of execution */
	this->retval = result;
	
	/* We're a zombie */
	this->zombie = TRUE;	
	
	/* Copying the next thread (to be executed) in ready queue */
	green_t *next = dequeue(&ready_queue);
	
	/* Replacing currently running thread with next thread in ready queue */ 
	running = next;
	
	/* Setting context */
	setcontext(next->context);
	
	sigprocmask(SIG_UNBLOCK, &block, NULL);
}
 
/* Initializes a green thread */
/* green_create(thread, function, arguments) */ 
int green_create(green_t *new, void *(*fun)(void*), void *arg){
	sigprocmask(SIG_BLOCK, &block, NULL);
	
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
	enqueue(&ready_queue, new);
	
	sigprocmask(SIG_UNBLOCK, &block, NULL);
	
	return 0;
}

/* Suspends current thread and selects a new thread for execution */
/* pthread_yield() causes the calling thread to relinquish (släppa) the CPU. The thread is placed at the end of the ready queue and another thread is scheduled to run. */
int green_yield(){
	sigprocmask(SIG_BLOCK, &block, NULL);
	
	/* Currently running process is copied */
	green_t *susp = running;
	
	/* Add susp last in ready queue (thread suspended) */
	enqueue(&ready_queue, susp);
	
	/* Copying the next thread (to be executed) from ready queue */
	green_t *next = dequeue(&ready_queue);
	
	/* Replacing currently running thread with next thread in ready queue */ 
	running = next;
	
	/* Swapping context. It will save current state in susp->context and continue execution from next->context. */
	swapcontext(susp->context, next->context);
	
	sigprocmask(SIG_UNBLOCK, &block, NULL);
	
	return 0;
}

/* Suspends current thread waiting for a thread to terminate */
/* pthread_join() function waits for the thread specified by argument to terminate. If that thread has already terminated, then pthread_join() returns immediately. */
int green_join(struct green_t *thread, void **res){
	sigprocmask(SIG_BLOCK, &block, NULL);

	/* If thread have not yet terminated... make sure it is allowed to run again */
	if(!thread->zombie) {
	
		green_t *susp = running;
		
		/* Add as joining thread */
		thread->join = susp;
		
		/* Select next thread for execution */
		green_t *next = dequeue(&ready_queue);
		running = next;
		swapcontext(susp->context, next->context);
	}
	
	/* Collect result */
	if(thread->retval != NULL){
		*res = thread->retval;
	}
	
	/* Free context */
	free(thread->context);
	
	sigprocmask(SIG_UNBLOCK, &block, NULL);
	
	return 0;
}







/******************************************
 *                                        *
 *           3. CONDITIONALS              *
 *                                        *
 ******************************************/

/* Initialize a green condition variable */
/* condition variable = used to wait for a condition to become true. Implemented as an explicit queue that threads can put themselves on to wait when some state is not as desired */
void green_cond_init(green_cond_t* cond){
	sigprocmask(SIG_BLOCK, &block, NULL);
	
	cond->susp_list = NULL;
	
	sigprocmask(SIG_UNBLOCK, &block, NULL);
}

/* This function is executed when a thread wishes to put it self to sleep. Suspends the current thread on the condition. The responsibility of wait() is to release the lock and put the calling thread to sleep (atomic)............ */
/* Release lock, put thread to sleep until cond is signaled. When thread wakes up again: re-aquire lock before returning. */
void green_cond_wait(green_cond_t* cond, green_mutex_t *mutex){
	sigprocmask(SIG_BLOCK, &block, NULL);
	
	/* Currently running process is copied */
	green_t *susp = running;
	assert(susp != NULL);
	
	/* Add susp thread to the list (condition) */
	enqueue(&cond->susp_list, susp);
	
	if(mutex != NULL){
		// Release the lock if we have a mutex
		mutex->taken = FALSE;
		
		// Move suspended thread to ready queue
		green_t *susp = dequeue(&mutex->susp);
		enqueue(&ready_queue, susp);
		mutex->susp = NULL;
		
	}
	
	/* Copying the next thread (to be executed) from ready queue */
	green_t *next = dequeue(&ready_queue);
	assert(next != NULL);
	
	/* Replacing currently running thread with next thread in ready queue */ 
	running = next;
	
	/* Swapping context. It will save current state in susp->context and continue execution from next->context. */
	swapcontext(susp->context, next->context);
	
	if(mutex != NULL){
		// Try to take the lock
		if(mutex->taken){
			// bad luck, suspend
			green_t *susp = running;
			enqueue(&mutex->susp, susp);
			
			green_t *next = dequeue(&ready_queue);
			running = next; 
			swapcontext(susp->context, next->context);
		} else {
			// take the lock
			mutex->taken = TRUE;
		}
	}
	
	sigprocmask(SIG_UNBLOCK, &block, NULL);
}
/* Some other waiting thread can be woken up to be allowed to execute  */
/* This function is executed when a thread has changed something in the program and wants to wake sleeping thread waiting on its condition. */
/* If any threads are waiting on cond, wake up one of them. Caller must hold lock which must be the same as the lock used in the wait call. */
void green_cond_signal(green_cond_t* cond){
	sigprocmask(SIG_BLOCK, &block, NULL);
	
	if(cond->susp_list == NULL){
		return;
	}
	
	green_t *thread = dequeue(&cond->susp_list);
	enqueue(&ready_queue, thread);
	
	sigprocmask(SIG_UNBLOCK, &block, NULL);
}




/******************************************
 *                                        *
 *           4. TIMER INTERRUPT           *
 *                                        *
 ******************************************/

void timer_handler(int sig){
	char *buf = "INTER\n\n";
	write(1, buf, sizeof(buf));
	
	green_yield();
}






/******************************************
 *                                        *
 *           5. MUTEX                     *
 *                                        *
 ******************************************/
 
int green_mutex_init(green_mutex_t *mutex){
	sigprocmask(SIG_BLOCK, &block, NULL);
 	
	mutex->taken = FALSE;
	mutex->susp = NULL;
	
	sigprocmask(SIG_UNBLOCK, &block, NULL);
}
 
 
int green_mutex_lock(green_mutex_t *mutex){
	sigprocmask(SIG_BLOCK, &block, NULL);
		
	if(mutex->taken){
		/* Suspend the running thread */
		green_t *susp = running;
		enqueue(&mutex->susp, susp);
		
		/* Find next thread */
		green_t *next = dequeue(&ready_queue);
		assert(next != NULL);
		
		running = next;
		swapcontext(susp->context, next->context);
	} else {
		/* Take lock */
		mutex->taken = TRUE;
	}
	
	sigprocmask(SIG_UNBLOCK, &block, NULL); 

	return 0;
}
 
int green_mutex_unlock(green_mutex_t *mutex){
	sigprocmask(SIG_BLOCK, &block, NULL);
	
	if(mutex->susp != NULL){
		/* Move susp thread to ready queue */
		green_t *susp = dequeue(&mutex->susp);
		enqueue(&ready_queue, susp);
		
	} else {
		/* Release lock */
		mutex->taken = FALSE;
		
		/* Reset susp */
		mutex->susp = NULL;
	}
	
	sigprocmask(SIG_UNBLOCK, &block, NULL);
	
	return 0;
}
