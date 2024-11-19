#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ulimit.h>
#include <sys/resource.h>
#include <string.h>

/*
-i Печатает реальные и эффективные идентификаторы пользователя и группы.
-s Процесс становится лидером группы. Подсказка: смотри setpgid(2)
-p Печатает идентификаторы процесса, процесса-родителя и группы процессов
-u Печатает значение ulimit
-Unew_ulimit Изменяет значение ulimit. Подсказка: смотри atol(3C) на странице руководства strtol(3C)
-c Печатает размер в байтах core-файла, который может быть создан.
-Csize Изменяет размер core-файла
-d Печатает текущую рабочую директорию
-v Распечатывает переменные среды и их значения
-Vname=value Вносит новую переменную в среду или изменяет значение существующей переменной
*/

int main(int argc, char **argv, char **envp)
{
  int aflag = 0;
  int bflag = 0;
  char *cvalue = NULL;
  int index;
  int c;

  opterr = 0;

  while ((c = getopt(argc, argv, "ispucdvU:C:V:")) != -1)
  {
    switch (c)
    {
    case 'i':
    {
      // -i Печатает реальные и эффективные идентификаторы пользователя и группы.
      printf("Real UID: %d\nEffective UID: %d\nReal GID: %d\nEffective GID: %d\n", getuid(), geteuid(), getgid(), getegid());
      break;
    }
    case 's':
    {
      // -s Процесс становится лидером группы. Подсказка: смотри setpgid(2)
      setpgid(0, 0);
      // The first argument is the PID of the calling process (0 can be passed to refer to the calling process itself).
      // The second argument is also the PID of the calling process, which will make the process the leader of a new process group.
      break;
    }
    case 'p':
    {
      // -p Печатает идентификаторы процесса, процесса-родителя и группы процессов
      // getpid - get the process ID
      // getppid - get the parent process ID
      // getpgrp -- get the process group ID of the calling process
      printf("Process ID: %d\nParent PID: %d\nGroup PID: %d\n", getpid(), getppid(), getpgrp());
      break;
    }
    case 'u':
    {
      // -u Печатает значение ulimit. ulimit - get and set process limits
      printf("ulimit: %ld\n", ulimit(UL_GETFSIZE));
      break;
    }
    case 'U':
    {
      // -Unew_ulimit Изменяет значение ulimit. Подсказка: смотри atol(3C) на странице руководства strtol(3C)
      // atol(optarg)
      // rlim_t -- Unsigned integer type used for limit values
      // rlim_t limit;
      printf("new ulimit: %ld\n", ulimit(UL_SETFSIZE, atol(optarg)));
      // printf("Ulimit: %ld\n", ulimit(UL_GETFSIZE));
      break;
    }
    case 'c':
    {
      // -c Печатает размер в байтах core-файла, который может быть создан.
      struct rlimit rlim;
      getrlimit(RLIMIT_CORE, &rlim);
      printf("Core file size: %llu\n", rlim.rlim_cur);
      break;
    }
    case 'C':
    {
      // -Csize Изменяет размер core - файла
      struct rlimit rlim;
      rlim.rlim_cur = atol(optarg);
      setrlimit(RLIMIT_CORE, &rlim);
      printf("Core file size changed to: %llu\n", rlim.rlim_cur);
      break;
    }
    case 'd':
    {
      // -d Печатает текущую рабочую директорию
      char dir[2000];
      getcwd(dir, 2000);
      printf("%s\n", dir);
      break;
    }
    case 'v':
    {
      // -v Распечатывает переменные среды и их значения
      for (char **env = envp; *env != 0; env++)
      {
        char *thisEnv = *env;
        printf("%s\n", thisEnv);
      }
      // printf("%s\n", getenv("HOME"));
      break;
    }
    case 'V':
    {
      // -Vname=value Вносит новую переменную в среду или изменяет значение существующей переменной
      setenv(optarg, strchr(optarg, '=') + 1, 1); // doesn't work
      break;
    }
    case '?':
    {
      if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      return 1;
    }
    default:
      abort();
    }
  }

  for (index = optind; index < argc; index++)
    printf("Non-option argument %s\n", argv[index]);
  return 0;
}
