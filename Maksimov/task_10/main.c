#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(const int argc, char** argv){
	if(argc == 1){
	printf("No command specified!\n");
	return -1;
	}

	pid_t p1;
	int fd[2];
	if((p1 = fork()) == 0){
		char command[256];
		execvp(argv[1], &argv[1]);
	}
	else if(p1 == -1){
		printf("An error occured!\n");
	}
	else{
		int status;
		if(waitpid(p1, &status, 0) == -1){
			perror("waitpid");
			return 1;
		}
		if(WIFEXITED(status)) printf("Return code = %d\n", WEXITSTATUS(status));
		else if(WIFSIGNALED(status)) printf("Termination code = %d\n", WTERMSIG(status));
	}
    return 0;
}
