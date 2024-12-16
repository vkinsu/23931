#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>


void print_file_info(const char *path) {
    struct stat file_stat;
    char perms[11] = "----------";
    struct passwd *pwd;
    struct group *grp;
    char time_str[20];
    char *file_name;

    if (lstat(path, &file_stat) == -1) {
        perror("information receiving error");
        return;
    }

    if (S_ISDIR(file_stat.st_mode))
        perms[0] = 'd';
    else if (S_ISREG(file_stat.st_mode))
        perms[0] = '-';
    else
        perms[0] = '?';

    if (file_stat.st_mode & S_IRUSR) perms[1] = 'r';
    if (file_stat.st_mode & S_IWUSR) perms[2] = 'w';
    if (file_stat.st_mode & S_IXUSR) perms[3] = 'x';

    if (file_stat.st_mode & S_IRGRP) perms[4] = 'r';
    if (file_stat.st_mode & S_IWGRP) perms[5] = 'w';
    if (file_stat.st_mode & S_IXGRP) perms[6] = 'x';

    if (file_stat.st_mode & S_IROTH) perms[7] = 'r';
    if (file_stat.st_mode & S_IWOTH) perms[8] = 'w';
    if (file_stat.st_mode & S_IXOTH) perms[9] = 'x';

    pwd = getpwuid(file_stat.st_uid);
    const char *owner = pwd ? pwd->pw_name : "unknown";

    grp = getgrgid(file_stat.st_gid);
    const char *group = grp ? grp->gr_name : "unknown";

    strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime));

    file_name = strrchr(path, '/');
    if (file_name)
        file_name++;
    else
        file_name = (char *)path;

    printf("%-10s %3ld %-8s %-8s %8ld %-12s %s\n",
           perms,
           file_stat.st_nlink,
           owner,
           group,
           S_ISREG(file_stat.st_mode) ? file_stat.st_size : 0,
           time_str,
           file_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Using: %s file1 [file2 ...]\n", argv[0]);
        return 1;
    }

    printf("%-10s %3s %-8s %-8s %8s %-12s %s\n",
           "type     ", "Links", "Owner  ", "Group  ", "Size  ", "Modification", "Name  ");

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}

