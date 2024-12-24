#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/unix_socket_example_gehrman"
#define BUFFER_SIZE 256

void to_uppercase(char *str)
{
    while (*str)
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
    return;
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Remove existing socket file
    unlink(SOCKET_PATH);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 1) == -1)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s...\n", SOCKET_PATH);

    // Accept a connection
    if ((client_fd = accept(server_fd, NULL, NULL)) == -1)
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Read data from client, convert to uppercase, and print
    ssize_t bytes_read;
    while ((bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1)) > 0)
    {
        buffer[bytes_read] = '\0';
        to_uppercase(buffer);
        printf("%s\n", buffer);
    }

    if (bytes_read == -1)
    {
        perror("Read error");
    }

    // Clean up
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    printf("Server terminated.\n");
    return 0;
}

