#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

main(int argc, char *argv[]){
    FILE *f;
    uid_t uid;
    uid = getuid();

    if(argc == 1){
        fprintf(stderr, "No filename.\n");
        exit(1);
    }
    printf("Real uid = %d and Effective uid = %d\n", uid, geteuid());
    f = fopen(argv[1], "r");
    if(f==NULL){
        perror(argv[0]);
        exit(1);
    }
    else{
        printf("Successfully opened.\n");
        fclose(f);
    }

    setuid(uid);
    printf("Real uid = %d and Effective uid = %d\n", uid, geteuid());
    f = fopen(argv[1], "r");
    if(f==NULL){
        perror(argv[0]);
        exit(1);
    }
    else{
        printf("Successfully opened.\n");
        fclose(f);
    }
    return 0;
}
