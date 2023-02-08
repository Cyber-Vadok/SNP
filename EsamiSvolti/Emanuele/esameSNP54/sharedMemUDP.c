/*
Esercizio

Un processo P1 agisce come server UDP. Quando riceve un messaggio di testo da
un qualsiasi client, scrive l'indirizzo IP del client e il messaggio di testo
ricevuto in due righe di una memoria condivisa POSIX. Un altro processo P2 deve leggere
ciascuna delle due righe dalla memoria condivisa e aggiungerle a un file. 
P1 e P2 devono gestire la concorrenza attraverso un meccanismo di blocco dei record.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <semaphore.h>

#define PORTUDP 49152
#define BUFSIZE 256
#define BACKLOG 10

//creazione del semaforo per la sincronizzazione
//sem_t semaphore;

//record lock
struct flock lock;
int fd;

//creazione del record lock in scrittura (necessita delle variabili globali)
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
  printf("\tShared memory: %p\n",shmem);
  return shmem;
}

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del server)
int udp_method(int sockfd,struct socklen_t *len){

	char buffer[BUFSIZE] = "";
	struct sockaddr_in client;
	char address[INET_ADDRSTRLEN] = "";
	int quit = 1;
	while (quit == 1){
		recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
		printf("\tRicevuto messaggio:'%s' da: %s:%d\n", buffer, inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN), ntohs(client.sin_port) );
		
		//inserimento nella memoria condivisa
		void* shmem = create_shared_memory(128);
		memcpy(shmem, buffer, sizeof(buffer));
		
		pid_t pid = fork();
  		if (pid == 0) { //child
  			printf("\tTaking the record lock ...\n");
  			printf("\tInserimento del messaggio nella memoria condivisa ... \n");
  			//sem_wait(&semaphore); // prendo il semaforo
  			lock.l_type = F_WRLCK;
  			fcntl(fd, F_SETLKW, &lock); //prendo il record lock
    		printf("\tP2 read from shared memory: %s\n", shmem);
    		FILE *fp;
			fp  = fopen ("data.txt", "a");
			for (int i = 0; i < strlen(buffer); i++) {
            /* write to file using fputc() function */
            fputc(buffer[i], fp);
        	}
        	fputc('\n',fp);
        	fclose(fp);
        	printf("\tMessaggio inserito nella memoria condivisa\n");
        	//sem_post(&semaphore); //rilascio il semaforo
        	lock.l_type = F_UNLCK; //rilascio il lock
			fcntl(fd, F_SETLK, lock);
        	printf("\tRecord lock released...\n\n");
        	exit(0);
    		//memcpy(shmem, child_message, sizeof(child_message));
    		//printf("Child process wrote: %s\n", shmem);
  		} else {
   	 		//printf("Parent read: %s\n", shmem);
   	 		//sleep(1);
    		//printf("After 1s, parent read: %s\n", shmem);
  		}
		if(waitpid(pid, NULL, 0) == pid){
			strncpy(buffer, "Messaggio inserito nella memoria condivisa", sizeof(buffer));
			sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client, len);
			sleep(1);
			strncpy(buffer, "", sizeof(buffer));
		}	
		
	}
	
	exit(0);
}

//Creazione del server UDP
int udp_server(int port){
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in server;
	socklen_t len = sizeof(server);
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address Family IPv4
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	bind(sockfd, (struct sockaddr *)&server, len);
	printf("\n\tServer UDP listening on port %d\n\n", (int)port);
	
	//Richiamo del metodo scritto da noi
	udp_method(sockfd,len);
	
	exit(0);
}


int main(int argc, char *argv[]){

	//inizializzazione del semaforo
	//il secondo parametro del semaforo è 0 se voglio che questo sia utilizzato solo dalle thread, invece un valore diverso da 0 se può essere utilizzato anche tra processi
	//il terzo parametro del semaforo è lo stato iniziale, metto 1 perchè voglio un semaforo binario
	//sem_init(&semaphore, 1, 1);
	
	create_record_lock();
	int sockfd = udp_server(PORTUDP);
	//eliminazione del semaforo
	//sem_destroy(&semaphore);
	return 0;
}