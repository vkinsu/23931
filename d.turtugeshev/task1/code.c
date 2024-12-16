#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <errno.h>
#include <ulimit.h>

void print_ids() {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    gid_t real_gid = getgid();
    gid_t effective_gid = getegid();

    printf("Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);
    printf("Real GID: %d, Effective GID: %d\n", real_gid, effective_gid);
}

void set_group_leader() {
    if (setpgid(0, 0) == -1) {
        perror("setpgid failed");
    } else {
        printf("Process is now the group leader.\n");
    }
}

void print_process_ids() {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t pgid = getpgrp();

    printf("PID: %d, PPID: %d, PGID: %d\n", pid, ppid, pgid);
}

void print_ulimit() {
    long limit = ulimit(UL_GETFSIZE, 0);
    if (limit == -1) {
        perror("ulimit failed");
    } else {
        printf("Current ulimit: %ld\n", limit);
    }
}

void set_ulimit(long new_limit) {
    if (ulimit(UL_SETFSIZE, new_limit) == -1) {
        perror("Failed to set ulimit");
    } else {
        printf("New ulimit set to: %ld\n", new_limit);
    }
}

void print_core_size() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("getrlimit failed");
    } else {
        printf("Core file size limit: %lu bytes\n", rl.rlim_cur);
    }
}

void set_core_size(rlim_t new_size) {
    struct rlimit rl;
    rl.rlim_cur = new_size;
    rl.rlim_max = new_size;

    if (setrlimit(RLIMIT_CORE, &rl) == -1) {
        perror("Failed to set core size");
    } else {
        printf("Core file size limit set to: %lu bytes\n", new_size);
    }
}

void print_current_directory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current directory: %s\n", cwd);
    } else {
        perror("getcwd failed");
    }
}

void print_environment_variables() {
    extern char **environ;
    for (char **env = environ; *env != 0; env++) {
        char *thisEnv = *env;
        printf("%s\n", thisEnv);
    }
}

void set_environment_variable(char *name_value) {
    char *equal_sign = strchr(name_value, '=');
    if (equal_sign != NULL) {
        *equal_sign = '0'; // Split name and value
        char *value = equal_sign + 1;
        if (setenv(name_value, value, 1) == -1) {
            perror("Failed to set environment variable");
        } else {
            printf("Environment variable %s set to %s\n", name_value, value);
        }
    } else {
        fprintf(stderr, "Invalid format for environment variable. Use name=value.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "No arguments\n");
        fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-C size] [-d] [-v] [-V name=value]\n", argv[0]);
        exit(EXIT_FAILURE);    
    }


    int opt;
    
    while ((opt = getopt(argc, argv, "ispuU:cC:dvV:")) != -1) {
        switch (opt) {
            case 'i':
                print_ids();
                break;
            case 's':
                set_group_leader();
                break;
            case 'p':
                print_process_ids();
                break;
            case 'u':
                print_ulimit();
                break;
            case 'U':
                {
                    long new_limit = atol(optarg);
                    if (new_limit < 0) {
                        fprintf(stderr, "Invalid value for ulimit: %s\n", optarg);
                    } else {
                        set_ulimit(new_limit);
                    }
                }
                break;
            case 'c':
                print_core_size();
                break;
            case 'C':
                {
                    rlim_t new_size = atol(optarg);
                    if (new_size < 0) {
                        fprintf(stderr, "Invalid core size: %s\n", optarg);
                    } else {
                        set_core_size(new_size);
                    }
                }
                break;
            case 'd':
                print_current_directory();
                break;
            case 'v':
                print_environment_variables();
                break;
            case 'V':
                set_environment_variable(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-C size] [-d] [-v] [-V name=value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}
