#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

void fileInfo(const char* filepath) {
    struct stat fileStats;

    if (stat(filepath, &fileStats) == -1) {
        perror("stat");
        exit(-1);
    }

    char type = '?';
    if (S_ISDIR(fileStats.st_mode)) {
        type = 'd';
    } else if (S_ISREG(fileStats.st_mode)) {
        type = '-';
    }

    char accessRights[10] = "---------";
    const mode_t permissions = fileStats.st_mode;
    if (permissions & S_IRUSR) accessRights[0] = 'r';
    if (permissions & S_IWUSR) accessRights[1] = 'w';
    if (permissions & S_IXUSR) accessRights[2] = 'x';
    if (permissions & S_IRGRP) accessRights[3] = 'r';
    if (permissions & S_IWGRP) accessRights[4] = 'w';
    if (permissions & S_IXGRP) accessRights[5] = 'x';
    if (permissions & S_IROTH) accessRights[6] = 'r';
    if (permissions & S_IWOTH) accessRights[7] = 'w';
    if (permissions & S_IXOTH) accessRights[8] = 'x';

    struct passwd* userEntry = getpwuid(fileStats.st_uid);
    const char* userName = userEntry ? userEntry->pw_name : "unknown";

    struct group* groupEntry = getgrgid(fileStats.st_gid);
    const char* groupName = groupEntry ? groupEntry->gr_name : "unknown";

    char formattedTime[40];
    struct tm* modificationTime = localtime(&fileStats.st_mtime);
    if (!modificationTime || strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", modificationTime) == 0) {
        snprintf(formattedTime, sizeof(formattedTime), "unknown time");
    }

    const char* fileName = strrchr(filepath, '/');
    fileName = fileName ? fileName + 1 : filepath;

    printf("%c%s %2ld %10s %10s %9ld %s %s\n",
           type,
           accessRights,
           fileStats.st_nlink,
           userName,
           groupName,
           S_ISREG(fileStats.st_mode) ? fileStats.st_size : 0,
           formattedTime,
           fileName);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("too less argv");
        exit(-1);
    }

    fileInfo(argv[1]);
    exit(0);
}