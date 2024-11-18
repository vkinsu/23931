#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_file_info(const char *filepath) {
    struct stat file_stat;
    struct passwd *pw;
    struct group *gr;
    char timebuf[64];
    char file_type;
    char permissions[10] = "---------";
    const char *filename = strrchr(filepath, '/');
    filename = (filename) ? filename + 1 : filepath;

    // Получение информации о файле
    if (lstat(filepath, &file_stat) == -1) {
        perror("lstat");
        return;
    }

    // Определение типа файла
    if (S_ISDIR(file_stat.st_mode)) file_type = 'd';
    else if (S_ISREG(file_stat.st_mode)) file_type = '-';
    else file_type = '?';

    // Права доступа
    if (file_stat.st_mode & S_IRUSR) permissions[0] = 'r';
    if (file_stat.st_mode & S_IWUSR) permissions[1] = 'w';
    if (file_stat.st_mode & S_IXUSR) permissions[2] = 'x';
    if (file_stat.st_mode & S_IRGRP) permissions[3] = 'r';
    if (file_stat.st_mode & S_IWGRP) permissions[4] = 'w';
    if (file_stat.st_mode & S_IXGRP) permissions[5] = 'x';
    if (file_stat.st_mode & S_IROTH) permissions[6] = 'r';
    if (file_stat.st_mode & S_IWOTH) permissions[7] = 'w';
    if (file_stat.st_mode & S_IXOTH) permissions[8] = 'x';

    // Имя владельца и группы
    pw = getpwuid(file_stat.st_uid);
    gr = getgrgid(file_stat.st_gid);

    // Время последней модификации
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&file_stat.st_mtime));

    // Вывод информации
    printf("%c%s %3lu %-8s %-8s %6ld %s %s\n",
           file_type,
           permissions,
           file_stat.st_nlink,
           (pw) ? pw->pw_name : "UNKNOWN",
           (gr) ? gr->gr_name : "UNKNOWN",
           (file_type == '-') ? file_stat.st_size : 0,
           timebuf,
           filename);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return EXIT_SUCCESS;
}
