#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_domain_socket"

int main() {
    char server_message[1024] = "";

    // create the server socket
    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    // check if we can't create the socket
    if (client_socket == -1) {
        perror("\nxxxx socket xxxx\n");
        exit(-1);
    }

    // define the server address
    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path));
    // server_address.sun_port = htons(PORT);
    // server_address.sun_addr.s_addr = INADDR_ANY;

    int connect_socket = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connect_socket == -1) {
        perror("\nxxxx connect xxxx\n");
        exit(-1);
    }

   while (fgets(server_message, sizeof(server_message), stdin)) {
	int write_message = write(client_socket, server_message, strlen(server_message));
	if (write_message == -1) {
		perror("\nxxxx write xxxx\n");
		exit(-1);
	}
   	
   }

    close(client_socket);

    return 0;
}
