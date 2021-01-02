#include <ucontext.h>

/* Threads */
typedef struct green_t {
	ucontext_t *context; // context of thread
	void *(*fun)(void*); // function to be executed
	void *arg;	      // arguments to the function
	struct green_t *next;
	struct green_t *join;
	void *retval;
	int zombie; // indicates if the thread has terminated or not
} green_t;

void green_thread();
int green_create(green_t *thread, void *(*fun)(void *), void *arg);
int green_yield();
int green_join(green_t *thread, void ** val);


/* Queue */
typedef struct Queue{
	struct green_t *head;
	struct green_t *tail;
	//struct green_t *waiting;
	int length;
} Queue;

struct Queue *create_queue(void);
void enqueue(struct Queue *queue, void *thread);
struct green_t *dequeue(struct Queue *queue);


/* Conditionals */
typedef struct green_cond_t {
	struct Queue *susp_queue;
} green_cond_t;

void green_cond_init(green_cond_t *cond);
void green_cond_wait(green_cond_t *cond);
void green_cond_signal(green_cond_t *cond);


/* Mutex */
typedef struct green_mutex_t {
	volatile int taken;
	struct Queue *mutex_queue;
} green_mutex_t;

int green_mutex_init(green_mutex_t *mutex);
int green_mutex_lock(green_mutex_t *mutex);
int green_mutex_unlock(green_mutex_t *mutex);


