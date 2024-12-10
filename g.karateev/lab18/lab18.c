#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>

#define MAX_DATE_LEN 20


char get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return '-';
    if (S_ISDIR(mode)) return 'd';
    return '?';
}

// Функция для получения прав доступа
void get_permissions(mode_t mode, char *permissions) {
    permissions[0] = (mode & S_IRUSR) ? 'r' : '-'; // Чтение владельцем
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-'; // Запись владельцем
    permissions[2] = (mode & S_IXUSR) ? 'x' : '-'; // Исполнение владельцем
    permissions[3] = (mode & S_IRGRP) ? 'r' : '-'; // Чтение группой
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-'; // Запись группой
    permissions[5] = (mode & S_IXGRP) ? 'x' : '-'; // Исполнение группой
    permissions[6] = (mode & S_IROTH) ? 'r' : '-'; // Чтение другими
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-'; // Запись другими
    permissions[8] = (mode & S_IXOTH) ? 'x' : '-'; // Исполнение другими
    permissions[9] = '\0';
}


const char* get_basename(const char *path) {
    char *copy = strdup(path);
    const char *name = basename(copy);
    free(copy);
    return name;
}


void process_file(const char *filepath) {
    struct stat st;
    if (stat(filepath, &st) == -1) {
        perror(filepath);   
        return;
    }


    char file_type = get_file_type(st.st_mode);


    char permissions[10];
    get_permissions(st.st_mode, permissions);


    int links = st.st_nlink;

    // Получение имени владельца и группы
    struct passwd *pwd = getpwuid(st.st_uid);
    struct group *grp = getgrgid(st.st_gid);
    const char *owner = pwd ? pwd->pw_name : "unknown";
    const char *group = grp ? grp->gr_name : "unknown";


    char size_str[20] = "";
    if (file_type == '-') {
        snprintf(size_str, sizeof(size_str), "%ld", st.st_size);
    }


    char mod_time[MAX_DATE_LEN];
    strftime(mod_time, MAX_DATE_LEN, "%b %d %Y %H:%M", localtime(&st.st_mtime));

    const char *filename = get_basename(filepath);


    printf("%c%-9s %3d %-8s %-8s %10s %16s %s\n",
           file_type, permissions, links, owner, group, size_str, mod_time, filename);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("T Permissions Links Owner    Group    Size       Last Modified      Name\n");
    printf("- ---------- ----- -------- -------- ---------- ------------------ --------\n");

    for (int i = 1; i < argc; i++) {
        process_file(argv[i]);
    }

    return EXIT_SUCCESS;
}
