#include <stdio.h>
#include <termios.h>
#include <unistd.h>

//element for canonical
struct termios old, new;

void not_canonical_terminal_set(){
    /* Get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO, &old);
    /* Copy the old settings to new settings */
    new = old;
    /* Make changes to the new settings */
    new.c_lflag &= ~(ICANON | ECHO);
    /* Apply the new settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void reset_canonical_terminal(){
	/* Reapply the old settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}

void get_inupt_not_canonical(){
	char c;
	c = getchar();
    
    /* Clear the screen */
    printf("\033c");
	
    printf("%c\n",c);
	fflush(stdout);
}

int main() {

	not_canonical_terminal_set();

    int i = 0;
    while(i < 10){
        get_inupt_not_canonical();
        i++;
    }

	reset_canonical_terminal();

	return 0;
}
