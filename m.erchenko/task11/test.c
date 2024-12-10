#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int execvpe(const char *file, char *const argv[], char *const envp[]);

int main(int argc, char *argv[]) {
    char *new_argv[] = { "env", NULL };

    char *new_envp[] = {
        "MY_VARIABLE=HelloWorld",
        "PATH=/usr/bin:/bin",
        NULL
    };

    if (execvpe("env", new_argv, new_envp) == -1) {
        perror("execvpe failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
