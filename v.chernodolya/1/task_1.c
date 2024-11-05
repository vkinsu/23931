#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

extern char **environ;

int main(int argc, char *argv[])
{
  char options[ ] = "ispuU:cC:dvV:";
  int c;
  struct rlimit rlim;
  unsigned long ulimit_renewed;
  unsigned long core_size_renewed;
  char **pointer;
  while ((c = getopt(argc, argv, options)) != EOF) {
    switch (c) {
      case 'i':
        printf("uid = %d\neuid = %d\ngetgid = %d\ngetegid = %d\n", getuid(), geteuid(), getgid(), getegid());
        break;

      case 's':
        if (setpgid(0, 0) == 0) {
          printf("Group process leader is set\n");
        } else {
          perror("Could not set group leader process\n");
        }
        break;

      case 'p':
        printf("pid = %d\nppid = %d\npgid = %d\n", getpid(), getppid(), getpgid(0));
        break;

      case 'u':
        if (getrlimit(RLIMIT_FSIZE, &rlim) == 0) {
          printf("Ulimit value: %lu\n", rlim.rlim_cur);
        } else {
          perror("Failed to get ulimit value\n");
        }
        break;

      case 'U':
        ulimit_renewed = strtol(optarg, NULL, 10);
        if (ulimit_renewed <= 0) {
          perror("Failed to set ulimit value\n");
          break;
        }

        if (getrlimit(RLIMIT_FSIZE, &rlim) != 0) {
          perror("Failed to set ulimit value\n");
          break;
        }

        rlim.rlim_cur = ulimit_renewed;
        if (-1 == setrlimit(RLIMIT_FSIZE, &rlim)) {
          perror("Failed to set ulimit value\n");
        } else {
          printf("Ulimit value is set\n");
        }
        break;

      case 'c':
        if (getrlimit(RLIMIT_CORE, &rlim) == 0) {
          printf("Byte size of core-file: %lu\n", rlim.rlim_cur);
        } else {
          perror("Failed to get core-file size\n");
        }
        break;

      case 'C':
        core_size_renewed = strtol(optarg, NULL, 10);
        if (core_size_renewed <= 0) {
          perror("Failed to set core size\n");
          break;
        }

        if (getrlimit(RLIMIT_FSIZE, &rlim) != 0) {
          perror("Failed to set core size\n");
          break;
        }

        rlim.rlim_cur = core_size_renewed;
        if (setrlimit(RLIMIT_FSIZE, &rlim) == -1) {
          perror("Failed to set core size\n");
        } else {
          printf("Core size is set\n");
        }
        break;

      case 'd':
        printf("Current working directory is: %s\n", getcwd(NULL,100));
        break;

      case 'v':
        pointer = environ;
        while (*pointer != NULL) {
          printf("%s\n", *pointer);
          pointer++;
        }
        break;

      case 'V':
        if (putenv(optarg) == 0) {
          printf("New environmental variable is set\n");
        } else {
          perror("Failed to set new environmental variable\n");
        }
    }
  }

}
#include <sys/resource.h>

extern char **environ;

int main(int argc, char *argv[])
{
  char options[ ] = "ispuU:cC:dvV:"; 
  int c;
  struct rlimit rlim;
  while ((c = getopt(argc, argv, options)) != EOF) {
    switch (c) {
      case 'i':
        printf("uid = %d\neuid = %d\ngetgid = %d\ngetegid = %d\n", getuid(), geteuid(), getgid(), getegid());
        break;

      case 's':
        if (setpgid(0, 0) == 0) {
          printf("Group process leader is set\n");
        } else {
          perror("Could not set group leader process\n");
        }
        break;
      
      case 'p':
        printf("pid = %d\nppid = %d\npgid = %d\n", getpid(), getppid(), getpgid(0));
        break;

      case 'u':
        if (getrlimit(RLIMIT_FSIZE, &rlim) == 0) {
          printf("Ulimit value: %lu\n", rlim.rlim_cur);
        } else {
          perror("Failed to get ulimit value\n");
        }
        break;
      
      case 'U':
        unsigned long ulimit_renewed = strtol(optarg, NULL, 10);
        if (ulimit_renewed <= 0) {
          perror("Failed to set ulimit value\n");
          break;
        }

        if (getrlimit(RLIMIT_FSIZE, &rlim) != 0) {
          perror("Failed to set ulimit value\n");
          break;
        }

        rlim.rlim_cur = ulimit_renewed;
        if (-1 == setrlimit(RLIMIT_FSIZE, &rlim)) {
          perror("Failed to set ulimit value\n");
        } else {
          printf("Ulimit value is set\n");
        }
        break;
      
      case 'c':
        if (getrlimit(RLIMIT_CORE, &rlim) == 0) {
          printf("Byte size of core-file: %lu\n", rlim.rlim_cur);
        } else {
          perror("Failed to get core-file size\n");
        }
        break;

      case 'C':
        unsigned long core_size_renewed = strtol(optarg, NULL, 10);
        if (core_size_renewed <= 0) {
          perror("Failed to set core size\n");
          break;
        }
        
        if (getrlimit(RLIMIT_FSIZE, &rlim) != 0) {
          perror("Failed to set core size\n");
          break;
        }

        rlim.rlim_cur = core_size_renewed;
        if (setrlimit(RLIMIT_FSIZE, &rlim) == -1) {
          perror("Failed to set core size\n");
        } else {
          printf("Core size is set\n");
        }
        break;

      case 'd':
        printf("Current working directory is: %s\n", getcwd(NULL,100));
        break;

      case 'v':
        char **pointer = environ;
        while (*pointer != NULL) {
          printf("%s\n", *pointer);
          pointer++;
        }
        break;
      
      case 'V':
        if (putenv(optarg) == 0) {
          printf("New environmental variable is set\n");
        } else {
          perror("Failed to set new environmental variable\n");
        }
    }
  }
  
}
