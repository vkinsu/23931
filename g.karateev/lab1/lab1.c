#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>     
#include <sys/resource.h>  
#include <limits.h>        
#include <string.h>        

void print_ids() {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    gid_t real_gid = getgid();
    gid_t effective_gid = getegid();

    printf("Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);
    printf("Real GID: %d, Effective GID: %d\n", real_gid, effective_gid);
}

void print_process_info() {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t pgid = getpgid(0);

    printf("Process ID: %d, Parent Process ID: %d, Process Group ID: %d\n", pid, ppid, pgid);
}

void set_process_leader() {
    if (setpgid(0, 0) == -1) {
        perror("setpgid failed");
    } else {
        printf("Process became the leader of the group.\n");
    }
}

void print_ulimit() {
    struct rlimit limit;
    if (getrlimit(RLIMIT_FSIZE, &limit) == 0) {
        printf("Current file size limit: %ld bytes\n", limit.rlim_cur);
    } else {
        perror("getrlimit failed");
    }
}

void change_ulimit(const char *new_ulimit_str) {
    long new_ulimit = atol(new_ulimit_str);
    struct rlimit limit;

    if (new_ulimit < 0) {
        fprintf(stderr, "Invalid ulimit value\n");
        return;
    }

    if (getrlimit(RLIMIT_FSIZE, &limit) == 0) {
        limit.rlim_cur = new_ulimit;
        if (setrlimit(RLIMIT_FSIZE, &limit) == 0) {
            printf("ulimit changed to %ld bytes\n", new_ulimit);
        } else {
            perror("setrlimit failed");
        }
    } else {
        perror("getrlimit failed");
    }
}

void print_core_size() {
    struct rlimit limit;
    if (getrlimit(RLIMIT_CORE, &limit) == 0) {
        printf("Core file size limit: %ld bytes\n", limit.rlim_cur);
    } else {
        perror("getrlimit failed");
    }
}

void change_core_size(const char *size_str) {
    long new_size = atol(size_str);
    struct rlimit limit;

    if (new_size < 0) {
        fprintf(stderr, "Invalid core size value\n");
        return;
    }

    if (getrlimit(RLIMIT_CORE, &limit) == 0) {
        limit.rlim_cur = new_size;
        if (setrlimit(RLIMIT_CORE, &limit) == 0) {
            printf("Core file size changed to %ld bytes\n", new_size);
        } else {
            perror("setrlimit failed");
        }
    } else {
        perror("getrlimit failed");
    }
}

void print_working_directory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd failed");
    }
}

void print_env_variables() {
    extern char **environ;
    for (char **env = environ; *env != 0; env++) {
        printf("%s\n", *env);
    }
}

void change_env_variable(const char *name_value) {
    if (putenv((char *)name_value) == 0) {
        printf("Environment variable set: %s\n", name_value);
    } else {
        perror("putenv failed");
    }
}

int main(int argc, char *argv[]) {
    int opt;
    char *optstring = "ispucC:U:dvV:";

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'i':
                print_ids();
                break;
            case 's':
                set_process_leader();
                break;
            case 'p':
                print_process_info();
                break;
            case 'u':
                print_ulimit();
                break;
            case 'U':
                change_ulimit(optarg);
                break;
            case 'c':
                print_core_size();
                break;
            case 'C':
                change_core_size(optarg);
                break;
            case 'd':
                print_working_directory();
                break;
            case 'v':
                print_env_variables();
                break;
            case 'V':
                change_env_variable(optarg);
                break;
            default:
                fprintf(stderr, "Invalid option: -%c\n", optopt);
                exit(EXIT_FAILURE);
        }
    }

    if (optind == 1) {
        printf("No arguments provided.\n");
    }

    return 0;
}
