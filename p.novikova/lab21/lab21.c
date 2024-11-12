#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define BEL "\07"

int ans = 0;

void handler_sigint(int signum) {
	write(STDOUT_FILENO, BEL, 1);
	ans++;
	signal(SIGINT, handler_sigint);
}

void handler_sigquit(int signum) {
	char buffer[13];
	int length = snprintf(buffer, sizeof(buffer), "\n%d\n", ans);

	write(STDOUT_FILENO, buffer, length);
	exit(0);
}

int main() {
	signal(SIGINT, handler_sigint);
	signal(SIGQUIT, handler_sigquit);

	while (1) {
            pause();
        }
}

