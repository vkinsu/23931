#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv){
	printf("Real UID = %d. Effective UID = %d\n", getuid(), geteuid());

	if(argc == 1){
		printf("File is not specified!\n");
		exit(1);
	}

	FILE* f = fopen(argv[1], "r");

	if(!f){
		perror("3C");
	}
	fclose(f);

	setuid(2);

	printf("Real UID = %d. Effective UID = %d\n", getuid(), geteuid());

	if(argc == 1){
		printf("File is not specified!\n");
		exit(1);
	}

	f = fopen(argv[1], "r");

	if(!f){
		perror("3C");
	}
	fclose(f);

	return 0;
}
