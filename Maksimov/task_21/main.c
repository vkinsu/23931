#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int signal_count = 0;

void handle_sigint() {
    	signal_count++;
    	printf("\a");
	fflush(stdout);
	signal(SIGINT, handle_sigint);
}

void handle_sigquit() {
    	printf("SIGQUIT. Num SIGINTв: %d\n", signal_count);
    	exit(0);
}

int main() {
    	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
    	while (1) {
        	pause();
    	}

    	return 0;
}
