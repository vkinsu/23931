#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const char *default_socket_path = "./socket";

int main(int argc, char *argv[]) {
    struct sockaddr_un socket_address;
    char input_buffer[100];
    int socket_fd, bytes_read;

    const char *socket_file = (argc > 1) ? argv[1] : default_socket_path;

    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sun_family = AF_UNIX;

    if (*socket_file == '0') {
        *socket_address.sun_path = '0';
        strncpy(socket_address.sun_path + 1, socket_file + 1, sizeof(socket_address.sun_path) - 2);
    } else {
        strncpy(socket_address.sun_path, socket_file, sizeof(socket_address.sun_path) - 1);
    }

    if (connect(socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(STDIN_FILENO, input_buffer, sizeof(input_buffer))) > 0) {
        if (write(socket_fd, input_buffer, bytes_read) != bytes_read) {
            if (bytes_read > 0) {
                fprintf(stderr, "Partial data write\n");
            } else {
                perror("Write error");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}
