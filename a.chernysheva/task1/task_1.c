#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <getopt.h>
#include <unistd.h>
#include <ulimit.h>
extern char **environ;

int main(int argc, char *argv[]){
    char options[] = "ispudU:cC:vV:";
    int c, invalid = 0, iflg = 0, sflg = 0, pflg = 0, dflg = 0, 
        uflg = 0, Uflg = 0, cflg = 0, Cflg = 0, vflg = 0, Vflg = 0;
    extern char *optarg;
    extern int optind, opterr, optopt;
    struct passwd *p;
    char cwd[256];
    struct rlimit rlp;


    printf("argc equals %d\n", argc);
    while((c = getopt(argc, argv, options)) != EOF){
        switch(c){
            case 'i':
            iflg++;
            printf ("Real User ID = %d\n",getuid());
            printf ("Effective User ID = %d\n\n",geteuid());
            printf ("Real Group ID = %d\n",getgid());
            printf ("Effective Group ID = %d\n\n",getegid());
            break;
            case 's':
            sflg++;
            if(setpgid(0, 0) == 0) printf("-s is successful\n");
            else printf("-s is not successful\n");
            break;
            case 'p':
            pflg++;
            printf("pid = %d, ppid = %d, gpid = %d\n", getpid(), getppid(), getpgid(0));
            break;
            case 'u':
            uflg++;
            getrlimit(RLIMIT_NOFILE, &rlp);
            printf("Open Files Soft Limit: %4ld\n", rlp.rlim_cur);
            printf("Open Files Hard Limit: %4ld\n", rlp.rlim_max);
            break;
            case 'U':
            Uflg++;
            getrlimit(RLIMIT_NOFILE, &rlp);
            rlp.rlim_cur = atol(optarg);
            if (setrlimit(RLIMIT_NOFILE, &rlp) == -1)
                fprintf(stderr, "You Cant Change File Limit\n");
            break;
            case 'c':
            cflg++;
            getrlimit(RLIMIT_CORE, &rlp);
            printf("core soft limit: %4ld\n", rlp.rlim_cur);
            break;
            case 'C':
            Cflg++;
            getrlimit(RLIMIT_CORE, &rlp);
            rlp.rlim_cur = atol(optarg);
            if (setrlimit(RLIMIT_CORE, &rlp) == -1)
                fprintf(stderr, "You Cant Change Core File\n");
            break;
            case 'd':
            dflg++;
            getcwd(cwd, sizeof(cwd));
            printf("Current Directory Is: %s\n", cwd);
            break;
            case 'v':
            vflg++;
            printf("Environment Variables Are:\n");
            for(char** i = environ; *i != NULL; i++){
                printf("%s\n", *i);
            }
            break;
            case 'V':
            Vflg++;
            if(putenv(optarg) == 0) printf("Successfully change env variables\n");
            break;
            case '?':
            invalid++;
            break;
        }
    }
    printf("iflg equals %d\n", iflg);
    printf("sflg equals %d\n", sflg);
    printf("pflg equals %d\n", pflg);
    printf("uflg equals %d\n", uflg);
    printf("Uflg equals %d\n", Uflg);
    printf("cflg equals %d\n", cflg);
    printf("Cflg equals %d\n", Cflg);
    printf("dflg equals %d\n", dflg);
    printf("vflg equals %d\n", vflg);
    printf("Vflg equals %d\n", Vflg);
    printf("invalid equals %d\n", invalid);
    printf("optind equals to %d\n", optind);
    if(optind < argc)
        printf("next parametr = %s\n", argv[optind]);
    return 0;
}
