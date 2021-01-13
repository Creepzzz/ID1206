#include <ucontext.h>

// structure of one page table entry
typedef struct pte {
	int present;
} pte;

void init(int *sequence, int refs, int pages);
