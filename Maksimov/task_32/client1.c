#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 256

// char *socket_path = "./socket";
char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {
  char* filename = argv[1];
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
  	perror("open error");
  	exit(EXIT_FAILURE);
  }
  struct sockaddr_un addr;
  char buf[BUFFER_SIZE];
  int fd;

  // if (argc > 1) socket_path=argv[1];

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*socket_path == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path+1, socket_path+1, sizeof(addr.sun_path)-2);
  } else {
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
  }

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(EXIT_FAILURE);
  }

  // while(1) {
  //   if (write(fd, buf, strlen(buf)) == -1) {
  //     close(fd);
  //     perror("write error");
  //     exit(EXIT_FAILURE);
  //   }
  //   buf[0] = (buf[0] == 'Z') ? 'A' : buf[0] + 1;
  //   sleep(0.1);
  // }

  while (1) {
	if (fgets(buf, BUFFER_SIZE, fp) != NULL) {
	    int write_message = write(fd, buf, strlen(buf));
	    if (write_message == -1) {
      perror("write error");
      close(fd);
      fclose(fp);
      exit(EXIT_FAILURE);
        }
    }
    
    if (feof(fp)) {
      rewind(fp);
    }
	
	  sleep(1);
  }

  close(fd);
  return 0;
}