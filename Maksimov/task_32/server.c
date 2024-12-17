#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MAX_EVENTS 5

char *socket_path = "\0hidden";

int set_nonblocking_socket(int fd){
  int flags = fcntl(fd, F_GETFL, 0);
  if(flags == -1) return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char *argv[]) {
    struct sockaddr_un addr;
    char buf[100];
    int server_fd, client_fd, rc, epoll_fd;
    int active_clients = 0;
    
    struct epoll_event ev, events[MAX_EVENTS];

    // Создаем сокет
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
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
        unlink(socket_path);
    }

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind error");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen error");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if(set_nonblocking_socket(server_fd) == -1){
      perror("socket nonblock error");
      close(server_fd);
      exit(EXIT_FAILURE);
    }

    if((epoll_fd = epoll_create1(0)) == -1){
      close(server_fd);
      perror("epoll create error");
      exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1){
      close(server_fd);
      close(epoll_fd);
      perror("epoll_clt error");
      exit(EXIT_FAILURE);
    }

    while (1) {
      int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
      if(nfds == -1){
        close(server_fd);
        close(epoll_fd);
        perror("epoll wait error");
        exit(EXIT_FAILURE);
      }

      for(int i = 0; i < nfds; i++){
        if(events[i].data.fd == server_fd){
          while((client_fd = accept(server_fd, NULL, NULL)) != -1){
            active_clients++;
            set_nonblocking_socket(client_fd);
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = client_fd;
            if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1){
              close(client_fd);
              active_clients--;
              // perror("epoll ctl add client error");
            }
          }
          if(errno != EAGAIN && errno != EWOULDBLOCK) perror("accept error");
        }
        else{
          int client_fd = events[i].data.fd;
          rc = read(client_fd, buf, sizeof(buf));
          if(rc > 0){
            for(int j = 0; j < rc; j++) buf[j] = toupper(buf[j]);
            // printf("Client %d: %.*s\n", client_fd, rc, buf);
            printf("%.*s", rc, buf);
          }
          else{
            if(rc == 0) printf("Client %d disconnected\n", client_fd);
            else perror("read error");
            close(client_fd);
            active_clients--;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);

            if(!active_clients){
              close(server_fd);
              close(client_fd);
              unlink(socket_path);
              exit(0);
            }
          }
        }
      }
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}
