#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){
    time_t now;
    time(&now);
    setenv("TZ", "America/Los_Angeles", 1);
    printf("%s", ctime(&now));
    exit(0);
}