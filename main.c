#include <stdlib.h>		// atexit
#include <errno.h>		// errno
#include <unistd.h>		// unix standard system calls
#include <termios.h>	// terminal input/output structure
#include <ctype.h>		// iscntrl
#include <stdio.h>		// printf

struct termios original_termios;

void die(const char *s){
	perror(s);
	exit(1);
}

void Read(int fileno, void * buf, size_t count){
	if(read(fileno, buf, count) == -1 && errno != EAGAIN){
		die("read");
	}
}

void Tcsetattr(int fileno, int flag, const struct termios * termios_p){
	if(tcsetattr(fileno, flag, termios_p) == -1){
		die("tcsetattr");
	}
}

void Tcgetattr(int fileno, struct termios * termios_p){
	if(tcgetattr(fileno, termios_p) == -1){
		die("tcgetattr");
	}
}

void disableRawMode(){
	Tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enableRawMode(){
	Tcgetattr(STDIN_FILENO, &original_termios);
	atexit(disableRawMode);

	struct termios raw_termios = original_termios;

	// input flags
	raw_termios.c_iflag &= ~( IXON		// disable ctrl-s, ctrl-q default behaviour: stop/start data flow
							| ICRNL		// disable ctrl-m default behaviour: translate to '13' (carriage return)
							| BRKINT	// break condition cause SIGINT
							| INPCK		// enable parity checking
							| ISTRIP	// strip 8th bit of each input byte
							);
	// output flags
	raw_termios.c_oflag &= ~(OPOST);		// disable output processing (e.g. \n -> \r\n)
	// control flags
	raw_termios.c_cflag |= (CS8);		// set character size to 8 bits per byte
	// local flags
	raw_termios.c_lflag &= ~( ECHO		// do not echo input to terminal
							| ICANON	// do not buffer input - byte by byte read immediately
							| ISIG		// disable ctrl-c, ctrl-z default behaviour: send SIGINT, SIGTSTP
							| IEXTEN	// disable ctrl-v default behaviour
							);
	raw_termios.c_cc[VMIN] = 0;			// minimum number of bytes of input needed before read() can return
	raw_termios.c_cc[VTIME] = 1;		// read() timeout

	Tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_termios);
}

int main(){
	enableRawMode();

    while(1){
		char c = '\0';
        Read(STDIN_FILENO, &c, 1);

		if(iscntrl(c)){
			// write(STDOUT_FILENO, "\n-ctrl-\n", 8);
			printf("%d\r\n", c);
		} else{
			// write(STDOUT_FILENO, &c, 1);
			printf("%d ('%c')\r\n", c, c);
		}

		if(c == 'q'){
			break;
		}
    }

    return 0;
}
