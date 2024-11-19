#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

int main(int argc, char *argv[]) {
    int c = 0;

    while ((c = getopt(argc, argv, "ucdvpisU:C:V:")) != -1) {
        switch (c) {
            case 'u': {
                struct rlimit limit;
                getrlimit(RLIMIT_NOFILE, &limit);
                printf("Current limit: soft = %ld, hard = %ld\n", (long)limit.rlim_cur, (long)limit.rlim_max);
                break;
            }
            case 'c': {
                struct rlimit limit;
                getrlimit(RLIMIT_CORE, &limit);
                printf("Current core file size limit: %ld bytes\n", (long)limit.rlim_cur);
                break;
            }
            case 'd': {
                char directory[1000];
                getcwd(directory, sizeof(directory));
                printf("Current working directory: %s\n", directory);
                break;
            }
            case 'v': {
                char **sred = environ;
                while (*sred) {
                    printf("%s\n", *sred);
                    sred++;
                }
                break;
            }
            case 'p': {
                pid_t pid = getpid();
                pid_t ppid = getppid();
                pid_t pgid = getpgrp();
                printf("Идентификатор процесса: %d\n", pid);
                printf("Идентификатор родительского процесса: %d\n", ppid);
                printf("Идентификатор группы процессов: %d\n", pgid);
                break;
            }
            case 'i': {
                uid_t real_uid = getuid();
                uid_t effective_uid = geteuid();
                uid_t real_gid = getgid();
                gid_t effective_gid = getegid();
                printf("Реальный идентификатор пользователя: %d\n", real_uid);
                printf("Эффективный идентификатор пользователя: %d\n", effective_uid);
                printf("Реальный идентификатор группы: %d\n", real_gid);
                printf("Эффективный идентификатор группы: %d\n", effective_gid);
                break;
            }
            case 's': {
                pid_t pid = getpid();
                setpgid(pid, pid);
                if (getpgrp() == pid) {
                    printf("%d\n", pid);
                }
                break;
            }
            case 'U': {
                if (optarg) {
                    long new_limit = strtol(optarg, NULL, 10);
                    struct rlimit limit;
                    limit.rlim_cur = new_limit;
                    limit.rlim_max = new_limit; 
                    if (setrlimit(RLIMIT_NOFILE, &limit) == 0) {
                        printf("New file limit set: soft = %ld, hard = %ld\n", (long)limit.rlim_cur, (long)limit.rlim_max);
                    } else {
                        perror("setrlimit failed");
                    }
                } else {
                    printf("Отсутствует значение параметра\n");
                }
                break;
            }
            case 'C': {
                if (optarg) {
                    long new_core_limit = strtol(optarg, NULL, 10);
                    struct rlimit limit;
                    limit.rlim_cur = new_core_limit;
                    limit.rlim_max = new_core_limit; 
                    if (setrlimit(RLIMIT_CORE, &limit) == 0) {
                        printf("New core file size limit set: soft = %ld, hard = %ld\n", (long)limit.rlim_cur, (long)limit.rlim_max);
                    } else {
                        perror("setrlimit failed for core file size");
                    }
                } else {
                    printf("Отсутствует значение параметра\n");
                }
                break;
            }
            /*
            ./task -V MY_VAR=123
            eval $(./task -V MY_VAR=123)
            echo $MY_VAR
            */
            case 'V': {
                char *var = optarg;
                char *name = strtok(var, "=");
                char *value = strtok(NULL, "=");

                if (name && value) {
                    setenv(name, value, 1);
                    printf("export %s=%s\n", name, value);
                } else {
                    printf("Ошибка: неправильный формат. Используйте -V name=value\n");
                }
                break;
            }

            default:
                printf("Плохие опции\n");
                break;
        }
    }

    return 0;
}
