/*
Esercizio

Ognuna delle tre thread di un processo A intende trasferire ad un
processo B il contenuto di un proprio file della dimensione 100 KB,
utilizzando un’area di memoria, della dimensione di 20 byte, condivisa
con tale processo. Per farsi riconoscere dal processo B ogni thread fa
precedere la sequenza di byte posta nell’area da una propria sigla
caratteristica di 2 byte.

Ciascuna thread del processo A può sovrascrivere l'area di memoria ogni
volta che riesce ad estrarre da una pipe un byte di valore prestabilito
che il processo B vi inserisce tutte le volte che ha effettuato con
successo la lettura di una nuova sequenza.

Scrivere tutto il software necessario ad implementare il meccanismo
sopra descritto facendo attenzione alla gestione della concorrenza tra
le thread e all'equa ripartizione tra le tre dell'accesso alla
scrittura.
*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <semaphore.h>

pthread_t tid[3];
pthread_mutex_t lock;
char* threadid_filecont[20];
void* shmem;
FILE *fileptr;
long fsize;
long mem_pointer = 0;

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

void readshmem(){
	char* id[21];
	strcpy(id,shmem);
	printf("I dati sono stati scritti dalla thread: %c\n", id[0]);
}

void* create_shared_memory_posix(char* name){
	
	int SIZE = 20;

	/* create the shared memory object */
	int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); 
	
	/* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE);
	
	/* memory map the shared memory object */
    void* ptr = mmap(0, SIZE, PROT_WRITE | PROT_READ , MAP_SHARED, shm_fd, 0);
	
	return ptr;
}

void read_file(FILE *fileptr,int filelen,char* threadid){
	char* fileContentTMP[filelen+1]; //il +1 è per il terminatore di stringa

	//reset string
	memset(threadid_filecont, 0, sizeof(threadid_filecont));
	memset(shmem, 0, sizeof(shmem));

	fread(fileContentTMP, filelen, 1, fileptr); // Read 

	//add thread id to file information
	strcat(threadid_filecont,threadid);
	strcat(threadid_filecont,fileContentTMP);

	memcpy(shmem, threadid_filecont, sizeof(threadid_filecont));
	printf("%s\n",shmem);
	readshmem();
}

long get_len_file(FILE *fileptr){
	fseek(fileptr, 0, SEEK_END);
	fsize = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);  /* same as rewind(f); */
	printf("Len file: %ld\n\n",fsize);
	return fsize;
}

void thread_read(char* name){
	int BUFFSIZE = 80;
    char readbuffer[BUFFSIZE];

	while (mem_pointer < fsize)
	{	
		read_from_pipe(readbuffer,BUFFSIZE);
		if( strcmp(readbuffer,"stop writing in file") == 0){
			pthread_mutex_lock(&lock); //prendo il mutex
			printf("%s ha preso il mutex\n",name);
			mem_pointer = mem_pointer + 18;
			read_file(fileptr,18,name);
			printf("%s ha lasciato il mutex\n\n",name);
			write_in_pipe("writing in file");
			pthread_mutex_unlock(&lock); //rilascio il mutex
		}
		memset(readbuffer, 0, sizeof(readbuffer));
	}
}





int main(int argc, char *argv[]){

	int BUFFSIZE = 80;
	char readbuffer[BUFFSIZE];

	//set pipe
	pipe(pipefd); 
	write_in_pipe("stop writing in file");
	

	pthread_mutex_init(&lock, NULL); //inizializzo il mutex
	shmem = create_shared_memory_posix("memCond");
	fileptr = fopen("myfile.txt", "r");
	get_len_file(fileptr);

	//process B
	pid_t pid = fork();
	if (pid == 0) { 
		printf("eccomi");
		while(1){
			printf("eccomi");
			read_from_pipe(readbuffer,BUFFSIZE);
			printf("%s",readbuffer);
			if(strcmp(readbuffer,"writing in file") == 0){
				int num;
				FILE *fptr;
				fptr = fopen("myfileB.txt","wa");
				fprintf(fptr,"%s",shmem);
				fclose(fptr);
				printf("Il processo B ha scritto sul file !\n");
				write_in_pipe("stop writing in file");
			}
			memset(readbuffer, 0, sizeof(readbuffer));
			sleep(1);
		}
	} else {
		pthread_create(&(tid[0]), NULL, &thread_read, "AT"); //faccio partire la thread
		pthread_create(&(tid[1]), NULL, &thread_read, "BT"); //faccio partire la thread
		pthread_create(&(tid[2]), NULL, &thread_read, "CT"); //faccio partire la thread

		pthread_join(tid[0], NULL); //aspetto che finisca la thread
		pthread_join(tid[1], NULL); //aspetto che finisca la thread
		pthread_join(tid[2], NULL); //aspetto che finisca la thread
	}
	
	return 0;
}