#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ulimit.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

<<<<<<< HEAD
extern char** environ;
=======
extern char **environ;
>>>>>>> 9fbc56df3b9b7dc097d0a47e99489a56ae276dd8

void print_id() {
    uid_t uid = getuid(); // реальный айди юзера
    uid_t euid = geteuid(); // эффективный айди юзера
    gid_t gid = getgid(); // реальный айди группы
    gid_t egid = getegid(); // эффективный айди группы

    printf("UID: %d, EUID: %d\n", uid, euid);
    printf("GID: %d, EGID: %d\n", gid, egid);
}

void set_pid_pgid() {
    pid_t pid = getpid(); //идентификатор процесса

    if (setpgid(0, 0) == 0) {
        printf("Process %d is now the leader of its process group\n", pid);
    } else {
        perror("Failed to become process group leader\n");
    }
}

void print_pid_ppid() {
    pid_t pid = getpid(); //идентификатор процесса
    pid_t ppid = getppid(); // идентификатор процесса-родителя
    pid_t pgid = getpgid(pid); // идентификатор группы процессов

    printf("PID: %d\n", pid);
    printf("PPID: %d\n", ppid);
    printf("PGID: %d\n", pgid);
}

void print_cwd() {
    char path[200];
    getcwd(path, 200);
    printf("CWD: %s\n", path);
}

void print_env_vars() {
<<<<<<< HEAD
   // for (int i = 0; envp[i]; i++) printf("%s\n", envp[i]);
    for (char** p = environ; *p; p++) printf("%s\n", *p);
=======
    printf("environment variables are:\n");
    for (char **p = environ; *p; p++) printf("%s\n", *p);

>>>>>>> 9fbc56df3b9b7dc097d0a47e99489a56ae276dd8
}

void get_ulimit_value() {
    printf("Ulimit: %ld\n", ulimit(1, 0));
}

void set_new_ulimit(char* val) {
    if (ulimit(2, atol(val)) == -1)
        perror("You must be SU to increase ulimit\n");
}

struct rlimit limit;

void print_core_size() {
    getrlimit(RLIMIT_CORE, &limit);
    printf("Сore size: %ld\n", limit.rlim_cur);
}

void set_core_size(char* val) {
    getrlimit(RLIMIT_CORE, &limit);
    limit.rlim_cur = atol(val);
    if (setrlimit(RLIMIT_CORE, &limit) == -1) perror("You must be SU to increase core\n");
}

void set_env_ver(char* var) {
    putenv(var);
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "ispdvucU:C:V:")) != -1) {
        switch (opt) {
            case 'i':
                print_id();
                break;
            case 's':
                set_pid_pgid();
                break;
            case 'p':
                print_pid_ppid();
                break;
            case 'd':
                print_cwd();
                break;
            case 'v':
                print_env_vars();
                break;
            case 'u':
                get_ulimit_value();
                break;
            case 'c':
                print_core_size();
                break;
            case 'U':
                set_new_ulimit(optarg);
                break;
            case 'V':
                set_env_ver(optarg);
		break;
            case 'C':
                set_core_size(optarg);
                break;
            default: 
                perror("Unknown option......\n");
        }
    }

    return 0;
}
