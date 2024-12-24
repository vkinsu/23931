#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/unix_socket_example_gehrman"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10 // Maximum number of clients the server can handle

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
    int server_fd, client_fd, max_fd, activity;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds; // Set of file descriptors for select()

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
    if (listen(server_fd, MAX_CLIENTS) == -1)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s...\n", SOCKET_PATH);

    // Array to keep track of client sockets
    int client_sockets[MAX_CLIENTS] = {0};

    while (1)
    {
        // Clear the read_fds set
        FD_ZERO(&read_fds);

        // Add the listening socket to read_fds
        FD_SET(server_fd, &read_fds);
        max_fd = server_fd;

        // Add all active client sockets to read_fds
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] > 0)
            {
                FD_SET(client_sockets[i], &read_fds);
                if (client_sockets[i] > max_fd)
                {
                    max_fd = client_sockets[i];
                }
            }
        }

        // Use select() to monitor multiple sockets
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR)
        {
            perror("Select error");
        }

        // Check if there's a new connection on the listening socket
        if (FD_ISSET(server_fd, &read_fds))
        {
            if ((client_fd = accept(server_fd, NULL, NULL)) == -1)
            {
                perror("Accept failed");
                continue;
            }

            // Add the new client socket to the array
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = client_fd;
                    printf("New client connected (fd: %d)\n", client_fd);
                    break;
                }
            }
        }

        // Check each client socket for incoming data
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            client_fd = client_sockets[i];
            if (FD_ISSET(client_fd, &read_fds))
            {
                ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
                if (bytes_read > 0)
                {
                    buffer[bytes_read] = '\0';
                    to_uppercase(buffer);
                    // printf("Client %d: %s\n", client_fd, buffer);
                    printf("%s\n", buffer);
                }
                else if (bytes_read == 0) // Client disconnected
                {
                    printf("Client %d disconnected.\n", client_fd);
                    close(client_fd);
                    client_sockets[i] = 0;
                }
                else if (bytes_read < 0)
                {
                    perror("Read error");
                }
            }
        }
    }

    // Cleanup (this will never be reached in this example, but good practice)
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
