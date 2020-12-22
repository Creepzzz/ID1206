#include <ucontext.h>

/* Thread */
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

typedef struct Queue{
	struct green_t *head;
	struct green_t *tail;
	int length;
} Queue;
//} Queue, green_cond_t;

struct Queue *create_queue(void);

void enqueue(struct Queue *queue, void *thread);
struct green_t *dequeue(struct Queue *queue);
