#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<fcntl.h>

int main() {
    // 1
    uid_t uid = getuid(); 
    uid_t euid = geteuid();

    printf("UID: %d, EUID: %d\n", uid, euid);

    // 2
    int fd = open("closed.txt", O_RDONLY);

    if (fd != -1) close(fd);
    else perror("Error with open\n");

    // 3
    setuid(uid = getuid());

    // 4
    uid = getuid(); 
    euid = geteuid();

    printf("UID: %d, EUID: %d\n", uid, euid);

    fd = open("closed.txt", O_RDONLY);

    if (fd != -1) close(fd);
    else perror("Error with open\n");

    return 0;
}
