 #include <sys/types.h>
 #include <stdio.h>
 #include <time.h>
 #include <stdlib.h>
//  extern char *tzname[];

 int main(){
    time_t now;
    struct tm *sp;
    
    (void)time(&now);
    time_t seconds = 7 * 60 * 60;
    time_t dif = difftime(now, seconds);
    
    //printf("%s", ctime(&dif));
    
    sp = gmtime(&dif);
    printf("%d/%d/%02d %d:%02d\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year - 100, sp->tm_hour,
        sp->tm_min);

    exit(0);
    
 }
