#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "socket"
#define BUFFER_SIZE 2048
#define SPAM_DELAY 1 

int main() {
    char buffer[BUFFER_SIZE];
    int client_num;
    struct sockaddr_un address_socket;
    

    if ((client_num = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&address_socket, 0, sizeof(address_socket));
    address_socket.sun_family = AF_UNIX;
    strncpy(address_socket.sun_path, SOCKET_PATH, sizeof(address_socket.sun_path) - 1);

    if (connect(client_num, (struct sockaddr*)&address_socket, sizeof(address_socket)) == -1) {
        perror("Connect failed");
        close(client_num);
        exit(EXIT_FAILURE);
    }



    while (1) {
        snprintf(buffer, BUFFER_SIZE, "spam");
        if (send(client_num, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }
        sleep(SPAM_DELAY);
    }

    close(client_num);
    return 0;
}
