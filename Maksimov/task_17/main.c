#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

struct termios termios_flags;
void beeper(){
	if(write(STDOUT_FILENO, "\a", 1) == -1){
		perror("write");
		exit(EXIT_FAILURE);
	}
}

void disable_raw(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_flags);
}

void enable_raw(){
	if(tcgetattr(STDIN_FILENO, &termios_flags) == -1){
		perror("tcgetattr");
		exit(EXIT_FAILURE);
	}
	atexit(disable_raw);
	struct termios raw_termios = termios_flags;
	raw_termios.c_lflag &= 255 - (ICANON | ECHO);
	raw_termios.c_cc[VTIME] = 0;
	raw_termios.c_cc[VMIN] = 1;
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_termios) == -1){
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}

}

int main(){
	int pos = 0;
	char sym;
	enable_raw();
	char str[41];
	memset(str, 0, 41);
	char c_d = termios_flags.c_cc[VEOF];
	char c_w = termios_flags.c_cc[VWERASE];
	char kill = termios_flags.c_cc[VKILL];
	char erase = termios_flags.c_cc[VERASE];
	while(1){
		if(read(STDIN_FILENO, &sym, 1) == -1){
			perror("read");
			exit(EXIT_FAILURE);
		}

		if(sym == kill){
			pos = 0;
			str[pos] = 0;
			char* clr = "\r\033[K\r";
			if(write(STDOUT_FILENO, clr, strlen(clr)) == -1){
				perror("write");
				exit(EXIT_FAILURE);
			}
		}
		else if(sym == erase){
			if(pos > 0){
				int l;
				pos--;
				str[pos] = 0;
				char buff[45];
				l = snprintf(buff, sizeof(buff), "\r\033[K%s", str);
				if((l > 0) && (write(STDOUT_FILENO, buff, l) == -1)){
					perror("write");
					exit(EXIT_FAILURE);
				}
			}
		}
		else if(sym == c_w){
			int l;
			char buff[45];
			while(pos > 0 && str[pos - 1] == ' ') pos--;
			while(pos > 0 && str[pos - 1] != ' ') pos--;
			while(pos < 40 && str[pos] == ' ') pos++;
			str[pos] = 0;
			l = snprintf(buff, sizeof(buff), "\r\033[K%s", str);
			if((l > 0) && (write(STDOUT_FILENO, buff, l) == -1)){
				perror("write");
				exit(EXIT_FAILURE);
			}
		}
		else if(sym == '\n' || sym == '\r'){ // coud be \r\n
			str[pos] = 0;
			char res[43];
			res[0] = '\n';
			char* n = "\n";
			char* line_2_write = strcat(res, str);
			line_2_write = strcat(line_2_write, n);
			if(write(STDOUT_FILENO, line_2_write, 43) == -1){
				perror("write");
				exit(EXIT_FAILURE);
			}
			pos = 0;
			memset(str, 0, sizeof(str));
		}
		else if(pos < 40 && sym >= 32 && sym <= 126){
			str[pos++] = sym;
			str[pos] = '\0';
            		if(write(STDOUT_FILENO, &sym, 1) == -1){
                		perror("write");
                		exit(EXIT_FAILURE);
            		}
        	}
		else if(sym < 32 || sym > 126) beeper();
		else if(pos >= 40){
            		str[pos] = 0;
            		char n = '\n';
            		if(write(STDOUT_FILENO, &n, 1) == -1){
				perror("write");
				exit(EXIT_FAILURE);
            		}
            		if (write(STDOUT_FILENO, str, pos) == -1) {
                		perror("write");
                		exit(EXIT_FAILURE);
            		}
            		if (write(STDOUT_FILENO, &n, 1) == -1) {
                		perror("write");
                		exit(EXIT_FAILURE);
            		}
            		memset(str, 0, sizeof(str));
            		pos = 0;
        	}
		else if((sym == c_d) && (pos <= 0)) break;
	}
	return 0;
}
