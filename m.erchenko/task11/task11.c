// execvpe.c
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern char **environ;

static char *getenv_from_envp(const char *name, char *const envp[]) {
    size_t len = strlen(name);
    for (char *const *ep = envp; *ep != NULL; ++ep) {
        if (strncmp(*ep, name, len) == 0 && (*ep)[len] == '=') {
            return *ep + len + 1;
        }
    }
    return NULL;
}

int execvpe(const char *file, char *const argv[], char *const envp[]) {
    char *path, *path_copy, *dir, *fullpath;
    size_t file_len, dir_len;
    int eacces = 0;

    if (strchr(file, '/')) {
        execve(file, argv, envp);
        return -1;
    }

    path = getenv_from_envp("PATH", envp);
    if (!path) {
        path = "/bin:/usr/bin";
    }

    path_copy = strdup(path);
    if (!path_copy) {
        return -1;
    }

    file_len = strlen(file);
    dir = strtok(path_copy, ":");
    while (dir) {
        dir_len = strlen(dir);
        if (dir_len == 0) {
            dir = ".";
            dir_len = 1;
        }
        fullpath = malloc(dir_len + 1 + file_len + 1);
        if (!fullpath) {
            free(path_copy);
            return -1;
        }

        memcpy(fullpath, dir, dir_len);
        fullpath[dir_len] = '/';
        strcpy(fullpath + dir_len + 1, file);

        execve(fullpath, argv, envp);

        int saved_errno = errno;

        free(fullpath);

        if (saved_errno == EACCES) {
            eacces = 1;
        } else if (saved_errno != ENOENT && saved_errno != ENOTDIR) {
            free(path_copy);
            errno = saved_errno;
            return -1;
        }

        dir = strtok(NULL, ":");
    }

    free(path_copy);

    if (eacces) {
        errno = EACCES;
    } else {
        errno = ENOENT;
    }
    return -1;
}
