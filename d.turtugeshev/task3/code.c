#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>


int main() {
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());


    FILE *file = fopen("myfile.txt", "r");
    if (file == NULL)
        perror("Error opening file");
    else {
    	fclose(file);
	printf("File opened successfully\n");
    }

    if (setuid(getuid()) == -1) {
	perror("setuid error");
	return EXIT_FAILURE;
    }

    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());


    file = fopen("myfile.txt", "r");
    if (file == NULL)
        perror("Error opening file");
    else {
        fclose(file);
        printf("File opened successfully");
    }

    return EXIT_SUCCESS;
}

