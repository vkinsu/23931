#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // 1
    uid_t uid = getuid(); 
    uid_t euid = geteuid();

    printf("UID: %d, EUID: %d\n", uid, euid);

    // 2
    FILE* f = fopen("closed.txt", "r+");

    if (f) fclose(f);
    else perror("Ошибка при открытии файла....");

    // 3
    setuid(uid = getuid());

    // 4
    uid = getuid(); 
    euid = geteuid();

    printf("UID: %d, EUID: %d\n", uid, euid);

    f = fopen("closed.txt", "r+");

    if (f) fclose(f);
    else perror("Ошибка при открытии файла....");

    return 0;
}
