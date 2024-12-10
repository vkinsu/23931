#include <stdio.h>
#include <time.h>
#include <sys/types.h>
int main(){
	setenv("TZ", "PST8PDT", 1);
	time_t now;
	struct tm *sp;
	(void) time(&now);
	printf("%s", ctime (&now));
}
