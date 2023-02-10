#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
	//IN ALTERNATIVA CON LA OPEN
	
	int fd_file = open("file1.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
	char writebuf[128] = "primariga\nseconda riga\ne terzaaa\0";
	
	write(fd_file, writebuf, strlen(writebuf));
	
        close(fd_file);
	
}
