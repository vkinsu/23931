#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void print_file_info(const char *filename) {
    struct stat file_stat;
    if (stat(filename, &file_stat) == -1) {
        perror("stat");
        return;
    }

    // Биты состояния файла
    char file_type = '?';
    if (S_ISDIR(file_stat.st_mode)) {
        file_type = 'd';
    } else if (S_ISREG(file_stat.st_mode)) {
        file_type = '-';
    }

    // Права доступа
    char permissions[10];
    snprintf(permissions, sizeof(permissions), "%c%c%c%c%c%c%c%c%c",
             file_type,
             (file_stat.st_mode & S_IRUSR) ? 'r' : '-',
             (file_stat.st_mode & S_IWUSR) ? 'w' : '-',
             (file_stat.st_mode & S_IXUSR) ? 'x' : '-',
             (file_stat.st_mode & S_IRGRP) ? 'r' : '-',
             (file_stat.st_mode & S_IWGRP) ? 'w' : '-',
             (file_stat.st_mode & S_IXGRP) ? 'x' : '-',
             (file_stat.st_mode & S_IROTH) ? 'r' : '-',
             (file_stat.st_mode & S_IWOTH) ? 'w' : '-',
             (file_stat.st_mode & S_IXOTH) ? 'x' : '-');

    // Количество связей файла
    int link_count = file_stat.st_nlink;

    // Имена собственника и группы файла
    struct passwd *pwd = getpwuid(file_stat.st_uid);
    struct group *grp = getgrgid(file_stat.st_gid);
    const char *owner_name = pwd ? pwd->pw_name : "unknown";
    const char *group_name = grp ? grp->gr_name : "unknown";

    // Размер файла (если это обычный файл)
    char size_str[20];
    if (S_ISREG(file_stat.st_mode)) {
        snprintf(size_str, sizeof(size_str), "%lld", (long long)file_stat.st_size);
    } else {
        strcpy(size_str, "");
    }

    // Дата модификации файла
    struct tm *timeinfo = localtime(&file_stat.st_mtime);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", timeinfo);

    // Имя файла
    char *file_name = strrchr(filename, '/');
    file_name = file_name ? file_name + 1 : (char *)filename;

    // Вывод информации
    printf("%s %3d %-8s %-8s %10s %s %s\n",
           permissions, link_count, owner_name, group_name, size_str, time_str, file_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file1 [file2 ...]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}

