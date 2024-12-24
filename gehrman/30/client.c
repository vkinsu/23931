#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/unix_socket_example_gehrman"
#define BUFFER_SIZE 256

int main()
{
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Connect to server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection to server failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Send data to server
    printf("Enter text to send to the server (Ctrl+D to end):\n");
    while (fgets(buffer, BUFFER_SIZE, stdin))
    {
        if (write(client_fd, buffer, strlen(buffer)) == -1)
        {
            perror("Write error");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
    }

    // Clean up
    close(client_fd);

    printf("Client terminated.\n");
    return 0;
}

