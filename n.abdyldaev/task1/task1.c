#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>
#include <errno.h>
#include <ulimit.h>
#include <limits.h>

extern char **environ;


int main(int argc, char *argv[]) {
    struct rlimit rl;
    struct rlimit rlc;
    char cwd[PATH_MAX];
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-U") == 0 && i + 1 < argc) {
            ulimit(UL_SETFSIZE, atol(argv[i+1]));
            printf("ulimit changed to %ld\n", atol(argv[i+1]));
            i++;
        } else if (strcmp(argv[i], "-C") == 0 && i + 1 < argc) {
            getrlimit(RLIMIT_CORE, &rlc);
            rlc.rlim_cur = atol(argv[i+1]);
            setrlimit(RLIMIT_CORE, &rlc);
            i++;
        }else if (strcmp(argv[i], "-V") == 0 && i + 1 < argc) {
            char *name_value = argv[i + 1];
            char *equal_sign = strchr(name_value, '=');
            if (equal_sign != NULL) {
                *equal_sign = '\0';
                char *name = name_value;
                char *value = equal_sign + 1;
                if (setenv(name, value, 1) == -1) {
                    perror("setenv failed");
                } else {
                    printf("environment variable %s set to %s\n", name, value);
                }
            }
            i++;
        }
        else {
            switch (argv[i][1]) {
                case 'i':
                    printf("real uid = %d\n", getuid());
                    printf("effective uid = %d\n", geteuid());
                    printf("real gid = %d\n", getgid());
                    printf("effective gid = %d\n", getegid());
                    break;
                case 's':
                    if (setpgid(0, 0) == -1) {
                        perror("setpgid failed");
                    }
                    break;
                case 'p':
                    printf("pid: %d\n", getpid());
                    printf("ppid: %d\n", getppid());
                    printf("pgid: %d\n", getpgrp());
                    break;
                case 'u':
                    if (getrlimit(RLIMIT_FSIZE, &rl) == -1) {
                        perror("getrlimit failed");
                        return 1;
                    }
                    printf("current limit: %ld bytes\n", rl.rlim_cur);
                    printf("max limit: %ld bytes\n", rl.rlim_max);
                    break;
                case 'c':
                    if (getrlimit(RLIMIT_CORE, &rlc) == -1) {
                        perror("getrlimit failed");
                        return 1;
                    }
                    printf("current limit: %ld bytes\n", rlc.rlim_cur);
                    printf("max limit: %ld bytes\n", rlc.rlim_max);
                    break;
                case 'd':
                     if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        printf("Current working directory: %s\n", cwd);
                    } else {
                        perror("getcwd() error");
                        return 1;
                    }
                    break;
                case 'v':
                    for (char **env = environ; *env != 0; env++) {
                        printf("%s\n", *env);
                    }
                    break;
                default:
                    printf("Unknown option: %s\n", argv[i]);
                    break;
            }
        }
    }

    return 0;
}

