#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

int fdpipe[2];

int main(int argc, char *argv[]){

	int BUFFSIZE = 80;
	char readbuffer[BUFFSIZE];

	pipe(fdpipe);

	close(fdpipe[1]); //voglio solo leggere
	int fd = open("text.txt", O_RDONLY | O_CREAT);

	//rimpiazzoi il mio file descriptor con quello del file
	if(dup2(fd,fdpipe[0]) != fdpipe[0]){
		perror("error dup2");
	}

	read(fdpipe[0], readbuffer, BUFFSIZE);
	printf("Ho letto dalla pipe: %s",readbuffer);

	close(fdpipe[0]);

	return 0;
}