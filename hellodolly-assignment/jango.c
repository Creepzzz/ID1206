#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(){
	int pid = fork();
	
	if(pid == 0){
		// open quotes.txt in read/write
		int fd = open("quotes.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		
		// copy entry to position 1
		dup2(fd, 1);
//		     ^   ^
//		     |   └─ new file descriptor
//		     └───── old file descriptor

		// close since we don't use it anymore
		close(fd);
		
		// transform into a clone "boba" that will direct all output to file descriptor 1
		execl("boba", "boba", NULL);
		printf("this only happens on failure\n");
	} else {
		wait(NULL);
	}
	
	return 0;
}
