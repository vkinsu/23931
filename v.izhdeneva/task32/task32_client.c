#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_domain_socket"

int main(int argc, char* argv[]) {
    char *filename = argv[1];
    int delay = atoi(argv[2]);
    
    FILE *fp = fopen(filename, "r");
    if (fp <= 0) {
	perror("\nxxxx fopen xxxx\n");
	exit(-1);
    } else printf("fopen ok\n");
    char buffer[256] = "";
    int fseek_message = fseek(fp, 0, SEEK_END);
    if (fseek_message != 0) {
	perror("\nxxxx fseek xxxx\n");
	exit(-1);
    }
    int size_message = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int fread_message = fread(buffer, size_message, 1, fp);
    if (fread_message <= 0) {
	perror("\nxxxx fread xxxx\n");
	exit(-1);
    }
    buffer[fread_message] = '\0';

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

    while (1) {
	// int send_data = send(client_socket, buffer, strlen(buffer), 0);
	// if (send_data == -1) {
	   //  perror("\nxxxx send xxxx\n");
	    // close(client_socket);
	    // exit(-1);
	// }
	int write_message = write(client_socket, buffer, strlen(buffer));
	if (write_message == -1) {
		perror("\nxxxx write xxxx\n");
		exit(-1);
	}
	usleep(delay);
    }

    close(client_socket);

    return 0;
}
