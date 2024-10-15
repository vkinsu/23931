#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main() {
    const char *filename = "test.txt";
    FILE *file;
    
    printf("Before setuid:\n");
    printf("Real user ID: %d\n", (int)getuid());
    printf("Effective user ID: %d\n", (int)geteuid());

   
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file\n");
    } else {
        printf("File opened successfully.\n");
        fclose(file);
    }

    setuid(getuid());

    printf("\nAfter setuid:\n");
    printf("Real user ID: %d\n", (int)getuid());
    printf("Effective user ID: %d\n", (int)geteuid());

   
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file\n");
    } else {
        printf("File opened successfully again.\n");
        fclose(file);
    }

    return 0;
}
