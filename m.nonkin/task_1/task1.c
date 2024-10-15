#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <ulimit.h>
#include <stdlib.h>
#include <stdio.h>

extern char **environ;

int main(int argc, char *argv[]) {
    struct rlimit rlp;
    char **p;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-options]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        char* arg = argv[i];

        if (arg[0] == '-') {
            switch (arg[1]) {
                case 'i':
                    printf("real userid = %d\n", (int)getuid());
                    printf("effective userid = %d\n", (int)geteuid());
                    printf("real groupid = %d\n", (int)getgid());
                    printf("effective groupid = %d\n", (int)getegid());
                    break;

                case 's':
                    setpgid(0, 0);
                    break;

                case 'p':
                    printf("process number = %d\n", (int)getpid());
                    printf("parent process number = %d\n", (int)getppid());
                    printf("group process number = %d\n", (int)getpgid(0));
                    break;

                case 'U':
                    if ((i + 1) < argc) {  // ensure there is a next argument
                        if (ulimit(2, atol(argv[++i])) == -1) {  // SET_FSLIM
                            fprintf(stderr, "Must be super-user to increase ulimit\n");
                        }
                    } else {
                        fprintf(stderr, "Option -U requires an argument\n");
                    }
                    break;

                case 'u':
                    printf("ulimit = %ld\n", ulimit(1, 0));  // GET_FSLIM
                    break;

                case 'c':
                    getrlimit(RLIMIT_CORE, &rlp);
                    printf("core size = %ld\n", (long)rlp.rlim_cur);
                    break;

                case 'C':
                    if ((i + 1) < argc) {  // ensure there is a next argument
                        getrlimit(RLIMIT_CORE, &rlp);
                        rlp.rlim_cur = atol(argv[++i]);
                        if (setrlimit(RLIMIT_CORE, &rlp) == -1) {
                            fprintf(stderr, "Must be super-user to increase core\n");
                        }
                    } else {
                        fprintf(stderr, "Option -C requires an argument\n");
                    }
                    break;

                case 'd':
                    {
                        char cwd[1024];
                        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                            printf("current working directory is: %s\n", cwd);
                        } else {
                            perror("getcwd() error");
                        }
                    }
                    break;

                case 'v':
                    printf("environment variables are:\n");
                    for (p = environ; *p; p++) {
                        printf("%s\n", *p);
                    }
                    break;

                case 'V':
                    if ((i + 1) < argc) {  // ensure there is a next argument
                        putenv(argv[++i]);
                    } else {
                        fprintf(stderr, "Option -V requires an argument\n");
                    }
                    break;

                default:
                    fprintf(stderr, "Unknown option: -%c\n", arg[1]);
                    break;
            }
        } else {
            fprintf(stderr, "Invalid argument format: %s\n", arg);
        }
    }

    return EXIT_SUCCESS;
}
