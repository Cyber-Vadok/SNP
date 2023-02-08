#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

//file descriptor pipe (position 0 for reading, position 1 for writing)
//comunicazione bidirezionale
int pipefd[2];


//il file descriptor 0 è per leggere dallo stdin e il file descriptor 1 è per scrivere nello stdoutmake
//scrivo nella pipe
void write_in_pipe(char *writebuffer){
	printf("Ha scritto nella pipe, al file descriptor %d: %s\n",pipefd[1],writebuffer);
	write(pipefd[1], writebuffer, (strlen(writebuffer)+1));	
}

//leggo nella pipe
void read_from_pipe(char *readbuffer,int bufsize){
	strcpy(readbuffer,"");
	read(pipefd[0], readbuffer, bufsize);
	printf("Legge al file descriptor %d: %s \n\n",pipefd[0],readbuffer);
}

//viene impostato per leggere dallo stdin
//se leggi dallo stdin devi scrivere nel terminale come se fosse una scanf
void read_from_stdin(){
	//rimpiazzo il mio fd con stdin
	if(dup2(STDIN_FILENO, pipefd[0]) != pipefd[0]){
		perror("dup2 error stdin");
	}
}

//viene impostato per scrivere nello stdout
void write_in_stdout(){
	//rimpiazzo il mio fd con stdout
	if(dup2(STDOUT_FILENO, pipefd[1]) != pipefd[1]){
		perror("dup2 error stout");
	}
}

int main(int argc, char *argv[]){

	//set pipe
	pipe(pipefd); 


    int BUFFSIZE = 80;
    char writebuffer[BUFFSIZE];
    char readbuffer[BUFFSIZE];

	strcpy(writebuffer,"Hello!");
	
	printf("main: pipefs[0] = %d ; pipefs[1] = %d\n",pipefd[0],pipefd[1]);
	printf("\n");

	pid_t childpid = fork();
	if(childpid == 0){

		write_in_stdout();
		write_in_pipe(writebuffer);

	}else {
		
		read_from_stdin();
		read_from_pipe(readbuffer,BUFFSIZE);
		
	}

	close(pipefd[0]);
	close(pipefd[1]);

	return 0;
}