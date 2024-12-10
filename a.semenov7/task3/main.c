#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int main()
{
    //printf("%d\n", setuid(getuid()));;
    printf("real uid: %d\n", getuid());
    printf("effective uid: %d\n", geteuid());

    FILE* f = fopen("test.txt", "r");
    if(f == NULL)
        perror("You can't open this file, dude...\n");
    else
        fclose(f);

    setuid(getuid());
    printf("real uid: %d\n", getuid());
    printf("effective uid: %d\n", geteuid());
    f = fopen("test.txt", "r");
    if(f == NULL)
        perror("You can't open this file, dude...\n");
    else
        fclose(f);
    return 0;
}
