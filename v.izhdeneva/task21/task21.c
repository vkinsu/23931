#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int sigint_count = 0;

void handle_sigint(int sig) {
    signal(SIGINT, SIG_IGN);
    printf("\a");
    fflush(NULL);
    printf("\nReceived SIGINT (signal %d)\n", sig);
    sigint_count++;
    sigset(SIGINT, handle_sigint);
}

void handle_sigquit(int sig) {
    printf("\nSIGQUIT received. SIGINT was received %d times.\n", sigint_count);
    exit(0);
}

int main() {
    sigset(SIGINT, handle_sigint);
    sigset(SIGQUIT, handle_sigquit);

    while (1) {
        pause();
    }

    exit(0);
}


