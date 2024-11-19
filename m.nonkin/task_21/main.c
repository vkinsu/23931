#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile int signal_count = 0;

void handle_sigint(int sig) {
    signal_count++;
    printf("\a");
    fflush(stdout);
    signal(SIGINT, handle_sigint);
}
void handle_sigquit(int sig) {
    (void)sig;
    printf("\nSIGQUIT received. Total: %d\n", signal_count);
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

