#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <ulimit.h>
#include <sys/resource.h>
#include <string.h>

void print_ids() {
    printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());
    printf("Real GID: %d, Effective GID: %d\n", getgid(), getegid());
}

void make_group_leader() {
    if (setpgid(0, 0) == -1) {
        perror("setpgid error");
    } else {
        printf("Process became a session leader\n");
    }
}

void print_process_ids() {
    printf("Process ID: %d, \nParent Process ID: %d, \nProcess Group ID: %d\n",
           getpid(), getppid(), getpgrp());
}

void print_ulimit() {
    long ulimit_value = ulimit(UL_GETFSIZE);
    if (ulimit_value == -1) {
        perror("Failed to get ulimit");
    } else {
        printf("Ulimit: %ld\n", ulimit_value);
    }
}

void set_ulimit(long new_ulimit) {
    if (ulimit(UL_SETFSIZE, new_ulimit) == -1) {
        perror("Failed to set ulimit");
    } else {
        printf("New ulimit set to: %ld\n", new_ulimit);
    }
}

void print_core_size() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0) {
        printf("Core file size limit: %ld bytes\n", rl.rlim_cur);
    } else {
        perror("Getrlimit error");
    }
}

void set_core_size(rlim_t size) {
    struct rlimit rl;
    rl.rlim_cur = size;
    rl.rlim_max = size;
    if (setrlimit(RLIMIT_CORE, &rl) != 0) {
        perror("setrlimit");
    } else {
        printf("Core file size limit set to: %ld bytes\n", (long)size);
    }
}

void print_current_directory() {
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("Failed to get current working directory.\n");
    }
}

void print_env_vars() {
    extern char **environ;
    for (char **env = environ; *env != 0; env++) {
        printf("%s\n", *env);
    }
}

void set_env_var(char *name_value) {
    char *name = strtok(name_value, "=");
    char *value = strtok(NULL, "=");
    if (name && value) {
        if (setenv(name, value, 1) != 0) {
            perror("Setenv error.");
            return;
        }
        printf("Environment variable set: %s=%s\n", name, value);
    } else {
        perror("Invalid environment variable format\n");
    }
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'i':
                    print_ids();
                    break;
                case 's':
                    make_group_leader();
                    break;
                case 'p':
                    print_process_ids();
                    break;
                case 'u':
                    print_ulimit();
                    break;
                case 'U':
                    if (i + 1 < argc) {
                        set_ulimit(atol(argv[++i]));
                    } else {
                        fprintf(stderr, "Option -U requires an argument.\n");
                    }
                    break;
                case 'c':
                    print_core_size();
                    break;
                case 'C':
                    if (i + 1 < argc) {
                        set_core_size(atol(argv[++i]));
                    } else {
                        fprintf(stderr, "Option -C requires an argument.\n");
                    }
                    break;
                case 'd':
                    print_current_directory();
                    break;
                case 'v':
                    print_env_vars();
                    break;
                case 'V':
                    if (i + 1 < argc) {
                        set_env_var(argv[++i]);
                    } else {
                        fprintf(stderr, "Option -V requires an argument.\n");
                    }
                    break;
                default:
                    printf("Invalid option %c\n", argv[i][1]);
            }
        } else {
            printf("Invalid argument: %s\n", argv[i]);
        }
    }
    return 0;
}