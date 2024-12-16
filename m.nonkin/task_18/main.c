#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void print_file_info(const char *file_path) {
    struct stat file_stat;
    struct passwd *owner;
    struct group *group;
    char perms[11];
    char time_str[100];

    if (stat(file_path, &file_stat) == -1) {
        perror("stat");
        return;
    }

    perms[0] = (S_ISDIR(file_stat.st_mode)) ? 'd' : (S_ISREG(file_stat.st_mode)) ? '-' : '?';

    perms[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0'; 

    owner = getpwuid(file_stat.st_uid);
    group = getgrgid(file_stat.st_gid);
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime));

    if (S_ISREG(file_stat.st_mode)) {
        printf("%-11s %-3ld %-20s %-20s %-8lld %-16s %s\n", perms, file_stat.st_nlink, owner ? owner->pw_name : "?", group ? group->gr_name : "?",
               (long long)file_stat.st_size, time_str, strrchr(file_path, '/') ? strrchr(file_path, '/') + 1 : file_path);
    } else {
        printf("%-11s %-3ld %-20s %-20s %-8s %-16s %s\n", perms, file_stat.st_nlink, owner ? owner->pw_name : "?", group ? group->gr_name : "?",
               "", time_str, strrchr(file_path, '/') ? strrchr(file_path, '/') + 1 : file_path);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}

