#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/unix_socket_example_gehrman"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10

int server_fd;
int client_sockets[MAX_CLIENTS] = {0}; // Global array for client sockets

void to_uppercase(char *str)
{
    while (*str)
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

void sigio_handler(int signo)
{
    char buffer[BUFFER_SIZE];
    int client_fd;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_fd = client_sockets[i];
        if (client_fd > 0)
        {
            ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
            if (bytes_read > 0)
            {
                buffer[bytes_read] = '\0';
                to_uppercase(buffer);
                printf("Client %d: %s\n", client_fd, buffer);
            }
            else if (bytes_read == 0)
            {
                printf("Client %d disconnected.\n", client_fd);
                close(client_fd);
                client_sockets[i] = 0;
            }
            else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
            {
                perror("Read error");
                close(client_fd);
                client_sockets[i] = 0;
            }
        }
    }

    // Handle new connections
    if ((client_fd = accept(server_fd, NULL, NULL)) != -1)
    {
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] == 0)
            {
                client_sockets[i] = client_fd;
                printf("New client connected (fd: %d)\n", client_fd);
                return;
            }
        }
        // If no slots available, reject client
        printf("Max clients reached. Rejecting connection.\n");
        close(client_fd);
    }
}

void cleanup(int signo)
{
    close(server_fd);
    unlink(SOCKET_PATH);
    printf("Server terminated.\n");
    exit(0);
}

int main()
{
    struct sockaddr_un server_addr;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) == -1)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s...\n", SOCKET_PATH);

    // Setup SIGIO handler
    struct sigaction sa;
    sa.sa_handler = sigio_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGIO, &sa, NULL) == -1)
    {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    // Set non-blocking mode and assign SIGIO ownership
    fcntl(server_fd, F_SETFL, O_NONBLOCK | O_ASYNC);
    fcntl(server_fd, F_SETOWN, getpid());

    // Setup cleanup on SIGINT
    struct sigaction sa_cleanup;
    sa_cleanup.sa_handler = cleanup;
    sigemptyset(&sa_cleanup.sa_mask);
    sa_cleanup.sa_flags = 0;
    sigaction(SIGINT, &sa_cleanup, NULL);

    while (1)
    {
        pause(); // Wait for SIGIO signals
    }

    return 0;
}

