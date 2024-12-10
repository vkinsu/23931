#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_domain_socket"
#define BUFFER_SIZE 256

int main(int argc, char* argv[]) {
    char *filename = argv[1];
    int delay = atoi(argv[2]);
    
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
  	perror("\nxxxx fopen xxxx\n");
  	exit(-1);
    } else printf("fopen ok\n");

    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("\nxxxx socket xxxx\n");
        exit(-1);
    }

    // define the server address
    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path));
    // server_address.sun_port = htons(PORT);
    // server_address.sun_addr.s_addr = INADDR_ANY;

    int connect_socket = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connect_socket == -1) {
        perror("\nxxxx connect xxxx\n");
	close(client_socket);
        exit(-1);
    }

   char buffer[BUFFER_SIZE];

    while (1) {
	if (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
	    int write_message = write(client_socket, buffer, strlen(buffer));
	    if (write_message == -1) {
		perror("\nxxxx wirte xxxx\n");
		close(client_socket);
		fclose(fp);
		exit(-1);
	    }
	}
	
	if (feof(fp)) {
	   rewind(fp);
	}
	
	usleep(delay);
    }

    fclose(fp);
    close(client_socket);

    return 0;
}
