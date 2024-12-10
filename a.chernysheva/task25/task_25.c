#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdlib.h>

#define MSG_LEN 30

int main(int argc, char *argv[]) {

    pid_t ppid = getpid();

    int fd[2];  //Pointers to the descriptors will be placed here
                //fd[0] - to read end
                //fd[1] - to write end
                //(They are both bidirectional though)

    const char msg_out[MSG_LEN] = "SoME randOm MsG";

    pipe(fd);
    printf("Pipe was created.\n");
    pid_t pid = fork();
    
    if(pid > 0) {  //parent
        printf("%ld. A child process with pid %ld was created.\n", ppid, pid);
        close(fd[0]);
        write(fd[1], msg_out, MSG_LEN);
        printf("%ld. A message \"%s\" was send by the pipe.\nWaiting for the child...\n", ppid, msg_out);
        close(fd[1]);
        if(wait(NULL) == -1) { //parent is waiting while child ends
            perror("Parent waits");
            exit(-1);
        }
        printf("%ld. Child finished the process.\n", ppid);
    }
    else if(pid == 0) { //child
        pid_t cpid = getpid();
        close(fd[1]);
        char msg_in[MSG_LEN];
        read(fd[0], msg_in, MSG_LEN);
        char *ptr;
        for(ptr = msg_in; *ptr; ptr++){
            *ptr = toupper(*ptr);
        }
        printf("%ld. The message is: \"%s\".\n", cpid, msg_in);
        close(fd[0]);
    }
    else {   //if fork doesn't work
        perror("Fork");
        exit(-1);
    }

    return 0;
}
