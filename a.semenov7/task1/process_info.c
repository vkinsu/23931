#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <limits.h>

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
        perror("setpgid");
    } else {
        printf("Process is now the group leader.\n");
    }
}

void print_ids_process() {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t pgid = getpgrp();

    printf("Process ID: %d, Parent Process ID: %d, Process Group ID: %d\n", pid, ppid, pgid);
}

void print_ulimit() {
    struct rlimit limit;
    if (getrlimit(RLIMIT_FSIZE, &limit) == 0) {
        printf("Current file size limit (ulimit): soft = %lu, hard = %lu\n", limit.rlim_cur, limit.rlim_max);
    } else {
        perror("getrlimit");
    }
}

void set_ulimit(long new_limit) {
    struct rlimit limit;
    if (getrlimit(RLIMIT_FSIZE, &limit) == 0) {
        limit.rlim_cur = new_limit;
        if (setrlimit(RLIMIT_FSIZE, &limit) == -1) {
            perror("Failed to set ulimit");
        } else {
            printf("New file size limit set to: %ld\n", new_limit);
        }
    } else {
        perror("getrlimit");
    }
}

void print_core_size() {
    struct rlimit limit;
    if (getrlimit(RLIMIT_CORE, &limit) == 0) {
        printf("Current core file size limit: soft = %lu, hard = %lu\n", limit.rlim_cur, limit.rlim_max);
    } else {
        perror("getrlimit");
    }
}

void set_core_size(long new_size) {
    struct rlimit limit;
    if (getrlimit(RLIMIT_CORE, &limit) == 0) {
        limit.rlim_cur = new_size;
        if (setrlimit(RLIMIT_CORE, &limit) == -1) {
            perror("Failed to set core file size");
        } else {
            printf("New core file size limit set to: %ld\n", new_size);
        }
    } else {
        perror("getrlimit");
    }
}

void print_working_directory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd");
    }
}

void print_environment_variables() {
    extern char **environ;
    for (char **env = environ; *env != 0; env++) {
        char *thisEnv = *env;
        printf("%s\n", thisEnv);
    }
}

void set_environment_variable(const char *name, const char *value) {
    if (setenv(name, value, 1) == -1) {
        perror("Failed to set environment variable");
    } else {
        printf("Environment variable %s set to %s\n", name, value);
    }
}

int main(int argc, char *argv[]) {
    int opt;
    long new_ulimit = -1;
    long new_core_size = -1;
    char *env_name = NULL;
    char *env_value = NULL;

    while ((opt = getopt(argc, argv, "ispu:U:c:C:d:vV:")) != -1) {
        switch (opt) {
            case 'i':
                print_ids();
                break;
            case 's':
                set_group_leader();
                break;
            case 'p':
                print_ids_process();
                break;
            case 'u':
                print_ulimit();
                break;
            case 'U':
                new_ulimit = atol(optarg);
                if (new_ulimit < 0) {
                    fprintf(stderr, "Invalid value for ulimit: %s\n", optarg);
                } else {
                    set_ulimit(new_ulimit);
                }
                break;
            case 'c':
                print_core_size();
                break;
            case 'C':
                new_core_size = atol(optarg);
                if (new_core_size < 0) {
                    fprintf(stderr, "Invalid value for core size: %s\n", optarg);
                } else {
                    set_core_size(new_core_size);
                }
                break;
            case 'd':
                print_working_directory();
                break;
            case 'v':
                print_environment_variables();
                break;
            case 'V':
                env_name = strtok(optarg, "=");
                env_value = strtok(NULL, "=");
                if (env_name && env_value) {
                    set_environment_variable(env_name, env_value);
                } else {
                    fprintf(stderr, "Invalid format for environment variable: %s\n", optarg);
                }
                break;
            default:
                fprintf(stderr, "Invalid option: -%c\n", optopt);
                break;
        }
    }

    return 0;
    // ./process_info -i -p -u 1024 -C 2048 -VTEST=value

}

