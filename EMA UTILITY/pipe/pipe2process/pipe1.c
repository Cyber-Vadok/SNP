#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

int fdpipe[2];

int main(int argc, char *argv[]){

	int BUFFSIZE = 80;
	char writebuffer[BUFFSIZE];
	strcpy(writebuffer,"Ciao da pipe1\n");

	pipe(fdpipe);

	close(fdpipe[0]); //voglio solo scrivere
	int fd = open("text.txt", O_WRONLY | O_CREAT);

	//rimpiazzoi il mio file descriptor con quello del file
	if(dup2(fd, fdpipe[1]) != fdpipe[1]){
		perror("error dup2");
	}

	write(fdpipe[1], writebuffer, (strlen(writebuffer)+1));	
	printf("Ho scritto nella pipe: %s",writebuffer);

	close(fdpipe[1]);

	return 0;
}