#include <stdlib.h>
#include <stdio.h>
#include "rand.h"

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("usage: rand <loop>\n");
		exit(1);
	}
	int loop = atoi(argv[1]);
	for(int i = 0; i < loop; i++){
		// request a size
		int size = request();
		printf("%d\n", size);
	}
}

/* PRINTOUT: 

$ ./test 100 | sort -n

	8
	8
	8
	9
	9
	10
	10
	11
	12
	12
	13
	13
	14

	:

	3159
	3652
	3723

*/
