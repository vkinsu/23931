#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

int main(const int argc, const char** argv){
	if(argc < 2){
		printf("Specify a filename\n");
		exit(-1);
	}

	const char* filename = argv[1];
	int fd = open(filename, O_RDWR);
	if(fd == -1){
		printf("Failed to open file\n");
		exit(1);
	}

	if(flock(fd, LOCK_EX) != 0){
		printf("Failed to lock file\n");
		close(fd);
		exit(1);
	}

	printf("File %s locked\n", filename);

	char command[256];
	snprintf(command, sizeof(command), "nano %s", filename);

	int return_value = system(command);

	close(fd);

	if(return_value == -1){
		printf("Failed to run command\n");
		exit(1);
	}
	printf("Editor closed\n");
	return 0;
}
