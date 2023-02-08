/*
Esercizio

Scrivere il software necessario a realizzare il seguente sistema: due server TCP 
indipendenti collocati sulla stessa macchina, ricevono dai client stringhe di 
testo terminate da un "a capo" e le registrano in un'area di memoria condivisa 
curando di evitare che le stringhe si intersechino. 
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
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>


#define PORTTCP 49152
#define BUFSIZE 256
#define BACKLOG 10

struct flock lock;
int fd;

void create_record_lock(){
  lock.l_type    = F_WRLCK; // F_RDLCK; per la lettura
  lock.l_start   = 0;
  lock.l_whence  = SEEK_SET;
  lock.l_len     = 0;  //50; per la lettura       
}

void writefile(int fd,char* buffer){
	
	lock.l_type = F_WRLCK; //prendo il record lock
	if(fcntl(fd, F_SETLKW, &lock) == -1){
		printf("\tError to lock\n");
	}else{
		printf("\tLock preso..\n");
	}
	printf("\tScrivo nel file condiviso: %s\n",buffer);
	write(fd,buffer,strlen(buffer));
	write(fd,"\n",strlen("\n"));
	lock.l_type = F_UNLCK; //rilascio il lock
	if(fcntl(fd, F_SETLK, &lock) == -1){
		printf("\tError to unlock\n");
	}else{
		printf("\tLock lasciato..\n");
	}
}

void* create_shared_memory_posix(char* name){
	
	int SIZE = 4096;

	/* create the shared memory object */
	int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); 
	
	/* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE);
	
	/* memory map the shared memory object */
    void* ptr = mmap(0, SIZE, PROT_WRITE | PROT_READ , MAP_SHARED, shm_fd, 0);
	
	return ptr;
}

int tcp_server(int port){

	int sockfd = 0; /* listening socket del server TCP */
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); //tipologia di socket
	
	//definizione di un indirizzo IPv4 su cui il server deve mettersi in ascolto
	struct sockaddr_in addr;
	
	memset(&addr, 0, sizeof(addr)); //svuoto la struttura
	addr.sin_family= AF_INET;//IPv4 family
	addr.sin_addr.s_addr = htonl(INADDR_ANY); //Accetto ogni tipo di connessione
	addr.sin_port = htons(port); //porta di ascolto
	
	bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	listen(sockfd, BACKLOG); //mi metto in ascolto sul mio socket
	printf("\n\tServer TCP listening on port %d\n", (int)port);
	
	return sockfd;
}

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del server)
int tcp_method(int peerfd, struct sockaddr_in *peer_addr){
		
		char buf[BUFSIZE] = "";
		
		//mi ricavo l'indirizzo di chi si è connesso in notazione decimale puntata
		char clientaddr[INET_ADDRSTRLEN] = "";
		inet_ntop(AF_INET, &(peer_addr->sin_addr), clientaddr, INET_ADDRSTRLEN);
		printf("\tAccepted a new TCP connection from %s:%d\n", clientaddr, ntohs(peer_addr->sin_port));
		int fd = open("data.txt", O_WRONLY|O_APPEND); //O_RDONLY|O_APPEND
		recv(peerfd, buf, BUFSIZE-1, 0);
		
		void* shmem = create_shared_memory_posix("memCond");
		
		lock.l_type = F_WRLCK; //prendo il record lock
		if(fcntl(fd, F_SETLKW, &lock) == -1){
			printf("Error to lock\n");
		}else{
			printf("Lock preso..\n");
		}
		
		printf("Ho inserito nella memoria condivisa: %s",buf);
		strcat(buf,"\n");
		strcat(buf,shmem);
		memcpy(shmem, buf, sizeof(buf));
		printf("\nOra la memoria condivisa contiene: \n%s",buf);
		
		if(fcntl(fd, F_SETLK, &lock) == -1){
		printf("Error to unlock\n");
		}else{
			printf("Lock lasciato\n");
		}
		
		//writefile(fd,buf);
		strcpy(buf,"");
		send(peerfd, "Ho scritto nella memoria condivisa", strlen("Ho scritto nella memoria condivisa"), 0);
		close(fd);

		printf("Child %d terminating..\n\n", (int)getpid());
		exit(0);
}

//Gestione del server concorrente
void tcp_concurrent(int sockfd){

	//inizializzo la struttura con le informazioni del futuro client
	struct sockaddr_in peer_addr;
	socklen_t len = sizeof(peer_addr);
	int peerfd = 0; /* connected socket del client TCP */
	
	while (1){
		//accetto la connessione sul mio socket
		peerfd = accept(sockfd, (struct sockaddr *)&peer_addr, &len);
		if (fork() == 0){
			tcp_method(peerfd, &peer_addr);
		}
		close(peerfd);
	}
}

int main(int argc, char *argv[]){
	
	//eseguire il programma con ./1TCPS
	create_record_lock();
	int sockfd = tcp_server(PORTTCP);
	/* remove the shared memory object */
	shm_unlink("memCond");
	tcp_concurrent(sockfd);	



	return 0;
}