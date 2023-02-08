/*
Esercizio

Scrivere un programma C che lanci tre processi A, B e C. Ognuno dei tre
processi deve aggiungere continuamente, con un intervallo di 3 secondi,
in uno spazio di memoria condivisa, un record costituito da: 1) il
proprio PID; 2) un numero intero casuale; 3) l'istante della scrittura.

I tre processi devono gestire la concorrenza ed evitare che le scritture
dei record si intersechino, sfruttando il meccanismo del record locking
su di un file.

Alla pressione dei tasti Ctrl-C i tre processi devono essere terminati e
il contenuto della memoria condivisa deve essere stampato allo standard
output.
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>

void* shmem; //memoria condivisa
struct flock lock; //record lock

int counter_pid = 0; //counter il il salvataggio dei pid dei child
pid_t spid[3]; //salvataggio dei pid dei child

pid_t wpid; //waitpid per aspettare che i child finiscano
int status = 0; //cattura lo status dei child quando finiscono

static volatile int keepRunning = 1;

//salvataggio dei pid dei processi child
void handle_pid(pid_t pid){
	if (counter_pid < 3){
		spid[counter_pid] = pid;
		counter_pid++;
	}
}

//gestione del segnale SIGINT e kill dei processi child
void intHandler(int sig) {
	printf("\nHai premuto Ctrl + C\n");
    for(int i=0;i < counter_pid; i++){
    	printf("Uccisione del child %llu in corso...\n",spid[i]);
    	kill(spid[i],SIGKILL);
    }
    printf("L'ultimo elemento presente nella memoria condivisa è: %s",shmem);
}

char* create_single_string(char* name, char* pid, char* rand, char* time){
	char mystring[256] = "";
	strcat(mystring,name);
	strcat(mystring," - ");
	strcat(mystring,"My pid: ");
	strcat(mystring,pid);
	strcat(mystring," - ");
	strcat(mystring,"My rand: ");
	strcat(mystring,rand);
	strcat(mystring," - ");
	strcat(mystring,"My time: ");
	strcat(mystring,time);
	return mystring;
}

//use struct tm
char* get_time(){
  	time_t now = time(NULL);
  	struct tm * tm_struct = localtime(&now);
  	
  	//prendo i valori che mi interessano in interi e gli trasformo in stringa
  	int hour = tm_struct->tm_hour;
  	char* shour = malloc(3);   // ex. 14
	sprintf(shour, "%d", hour);
	
  	int min = tm_struct->tm_min;
  	char* smin = malloc(3);   // ex. 14
	sprintf(smin, "%d", min);
  	
  	int sec = tm_struct->tm_sec;
	char* ssec = malloc(3);   // ex. 14
	sprintf(ssec, "%d", sec);
	
	//raggruppo tutto in un unica stringa
	strcat(shour,":");
	strcat(shour,smin);
	strcat(shour,":");
	strcat(shour,ssec);
	
  	return shour;
}

char* get_mypid(){
	int pid = getpid();
	
	//trasformo in stringa
	char * mypid = malloc(6);   // ex. 34567
	sprintf(mypid, "%d", pid);
	return mypid;
}

char* get_randomNumber(){
	srand(time(NULL));  
	int r = rand() %100;
	
	//trasformo in stringa
	char* myrandom = malloc(3);   // ex. 99
	sprintf(myrandom, "%d", r);
	return myrandom;
}

void create_record_lock(){
  lock.l_type    = F_WRLCK; // F_RDLCK; per la lettura
  lock.l_start   = 0;
  lock.l_whence  = SEEK_SET;
  lock.l_len     = 0;  //50; per la lettura          
}

void* create_shared_memory(size_t size) {
  
  void* shmem;
  
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  
  shmem = mmap(NULL, size, protection, visibility, -1, 0);
  printf("Shared memory: %p\n",shmem);
  return shmem;
}

void* create_child(int fd, char* name){
	
	pid_t pid = fork();
	if (pid == 0){
		printf("Processo %s creato!\n\n",name);
		handle_pid(getpid());
		while (keepRunning == 1){
			lock.l_type = F_WRLCK; //prendo il record lock
			if(fcntl(fd, F_SETLKW, &lock) == -1){
				printf("Error to lock\n");
			}else{
				printf("%s ha preso il lock..\n",name);
			}
			char* mystring = create_single_string(name,get_mypid(),get_randomNumber(),get_time());
			printf("Il processo %s ha apportato una modifica.\n");
			memcpy(shmem, mystring, 256);
			printf("%s\n",shmem);
			lock.l_type = F_UNLCK; //rilascio il lock
			if(fcntl(fd, F_SETLK, &lock) == -1){
				printf("Error to unlock\n");
			}else{
				printf("%s ha lasciato il lock..\n\n",name);
			}
			
			sleep(3);
		}
		printf("Processo %s fermato!\n\n",name);
		exit(0);
	}
	
}

int main(int argc, char *argv[]){
	
	shmem = create_shared_memory(256);
	create_record_lock();
	int fd = open("esameSNP43.txt", O_RDWR);
	
	signal(SIGINT, intHandler);
	
	create_child(fd,"A");
	create_child(fd,"B");
	create_child(fd,"C");
	
	while ((wpid = wait(&status)) > 0); //aspetta che i child finiscano
	
	return 0;
}