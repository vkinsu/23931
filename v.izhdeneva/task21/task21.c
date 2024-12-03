#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t sigint_count = 0;

void handle_sigint(int sig) {
    printf("\a");
    fflush(stdout);
    printf("\nReceived SIGINT (signal %d)\n", sig);
    sigint_count++;
}

void handle_sigquit(int sig) {
    printf("\nSIGQUIT received. SIGINT was received %d times.\n", sigint_count);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);

    while (1) {
        pause();
    }

    exit(0);
}


