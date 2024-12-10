#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./test"
#define BUFFER_SIZE 1024

void to_upper_case(char *str){
    while (*str){
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];


    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1){
        perror("socket!!");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);


    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1){
        perror("bind!!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) == -1){
        perror("listen!!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("enter waiting %s\n", SOCKET_PATH);

    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1){
        perror("accept!!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //  data
    ssize_t len = read(client_fd, buffer, sizeof(buffer) - 1);
    if (len == -1){
        perror("read");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    buffer[len] = '\0';
    to_upper_case(buffer);
    printf("result: %s\n", buffer);


    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

