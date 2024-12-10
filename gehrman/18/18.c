#include <stdio.h>
#include <sys/stat.h> // "статистика" файла
#include <grp.h>      // поиск группы
#include <pwd.h>      // поиск владельца
#include <time.h>
#include <string.h> // для имени файла
#include <unistd.h> // для getcwd()
#include <limits.h> // для PATH_MAX

void print_file_info(char *path)
{
    struct stat file_stat;
    if (lstat(path, &file_stat) == -1)
    {
        perror("lstat");
        return;
    }

    // Определяем тип файла
    char file_type = '?';
    if (S_ISDIR(file_stat.st_mode))
        file_type = 'd';
    else if (S_ISREG(file_stat.st_mode))
        file_type = '-';

    // Определяем права
    char permissions[9] = "---------";
    if (file_stat.st_mode & S_IRUSR)
        permissions[0] = 'r';
    if (file_stat.st_mode & S_IWUSR)
        permissions[1] = 'w';
    if (file_stat.st_mode & S_IXUSR)
        permissions[2] = 'x';
    if (file_stat.st_mode & S_IRGRP)
        permissions[3] = 'r';
    if (file_stat.st_mode & S_IWGRP)
        permissions[4] = 'w';
    if (file_stat.st_mode & S_IXGRP)
        permissions[5] = 'x';
    if (file_stat.st_mode & S_IROTH)
        permissions[6] = 'r';
    if (file_stat.st_mode & S_IWOTH)
        permissions[7] = 'w';
    if (file_stat.st_mode & S_IXOTH)
        permissions[8] = 'x';

    // Количество связей
    nlink_t link_count = file_stat.st_nlink;

    // Получаем владельца и группу
    struct passwd *owner = getpwuid(file_stat.st_uid);
    struct group *grp = getgrgid(file_stat.st_gid);
    const char *owner_name = owner ? owner->pw_name : "unknown";
    const char *group_name = grp ? grp->gr_name : "unknown";

    // Получаем дату модификации
    char date[20];
    // struct tm time = ctime(&file_stat.st_mtime);
    struct tm *time = localtime(&file_stat.st_mtime);
    strftime(date, sizeof(date), "%b %d %Y %H:%M", time);

    // Получаем имя файла
    const char *file_name = strrchr(path, '/');
    file_name = file_name ? file_name + 1 : path;

    // Печатаем результат
    printf("%c", file_type);
    printf("%s\t", permissions);
    printf("%d\t", link_count);
    printf("%s\t", owner_name);
    printf("%s\t", group_name);

    printf("%lld\t", file_stat.st_size);
    printf("%s\t", date);
    printf("%s\n", file_name);

    return;
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Error: Usage: %s <file1> [file2 ...]\n", argv[0]);

        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("Current working dir: %s\n", cwd);
        }
        else
        {
            perror("getcwd() error");
            return 1;
        }

        print_file_info(cwd);
    }

    for (int i = 1; i < argc; i++)
    {
        print_file_info(argv[i]);
    }

    return 0;
}
