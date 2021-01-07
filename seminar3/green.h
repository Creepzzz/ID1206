#include <ucontext.h>

typedef struct green_t {
	ucontext_t *context; // context of thread
	void *(*fun)(void*); // function to be executed
	void *arg;	      // arguments to the function
	struct green_t *next;
	struct green_t *join;
	void *retval;
	int zombie; // indicates if the thread has terminated or not
} green_t;

typedef struct green_cond_t {
	struct green_t *susp_list;
} green_cond_t;

typedef struct green_mutex_t {
	volatile int taken;
	struct green_t *susp;
} green_mutex_t;


/* Green threads functions */
void green_thread(void);
int green_create(green_t *thread, void *(*fun)(void *), void *arg);
int green_yield();
int green_join(green_t *thread, void ** val);

/* Queue functions */ 
void enqueue(green_t **list, green_t *thread);
green_t *dequeue(green_t **list);

/* Conditional functions */ 
void green_cond_init(green_cond_t *cond);
void green_cond_wait(green_cond_t *cond, green_mutex_t *mutex);
void green_cond_signal(green_cond_t *cond);

/* Timer functions */
void timer_handler(int);

/* Mutex functions */
int green_mutex_init(green_mutex_t *mutex);
int green_mutex_lock(green_mutex_t *mutex);
int green_mutex_unlock(green_mutex_t *mutex);


