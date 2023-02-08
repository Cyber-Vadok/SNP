/*
Esercizio

Scrivere i due programmi C necessari a implementare il seguente sistema.
Su richiesta di un client, un server deve inviare uno stream continuo di
caratteri alfanumerici. L'utente del client deve poter regolare la
velocità di trasmissione dei caratteri con la sola pressione del tasto "u" 
per aumentarla e del tasto "d" per diminuirla. Note: a) la velocità di
trasmissione corrente deve essere misurata e mostrata al terminale; b)
usare l'indirizzo localhost per lo scambio dei dati tra i due processi;
c) i dati ricevuti devono essere salvati in un file.
*/

#include "Header.h"

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

//Creazione del client TCP
int tcp_client(int argc, char *argv[],int port){

	int sockfd = 0; //connection socket: servirà per la comunicazione col server
	
	struct sockaddr_in server; //IPv4 server address, senza hostname resolution 
	socklen_t len = sizeof(server); //dimensione della struttura di indirizzo
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address FAmily IPv4
	server.sin_port = htons(port);

	if (argc < 2){
		server.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //se voglio connettermi in localhost
	}else {
		inet_pton(AF_INET, argv[1], &(server.sin_addr)); //altrimenti salvo l'indirizzo ip passato come parametro in notazione decimale puntata
	}
	
	
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); //apro il socket del client
    connect(sockfd, (struct sockaddr *)&server, len); //mi connetto al server
    return sockfd;
    
}

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del client)
void tcp_method(int sockfd){

	struct timespec start, end;

	char msg[BUFSIZE] = " ";
	char msg1[BUFSIZE] = " ";
	//inserimento nella memoria condivisa
	void* shmem = create_shared_memory(128);
	memcpy(shmem, msg, sizeof(msg));

	pid_t pid = fork();
  	if (pid == 0) {
		while(1){
			scanf("%s", msg);
			memcpy(shmem, msg, sizeof(msg));  
			//printf("sono child %s\n",shmem);
			sleep(1);
			memcpy(shmem, " ", sizeof(" "));  
		}
	}else {
		while (1){

			send(sockfd, shmem, strlen(shmem), 0);
			clock_gettime(CLOCK_MONOTONIC_RAW, &start);
			recv(sockfd, msg1, BUFSIZE-1, 0);
			if(strcmp(msg1,"stop") == 0){
				printf("\nSTOP\n");
				kill(pid, SIGKILL);
				exit(0);
			}
			clock_gettime(CLOCK_MONOTONIC_RAW, &end);
			uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
			printf("Messaggio %s arrivato in: %llu millisecond\n",msg1,delta_us);
			strcpy(msg1,"");
			//sleep(1);
		}
    		close(sockfd);

	}

}


int main(int argc, char *argv[]){
	
	int sockfd = tcp_client(argc,argv,PORTTCP);
	tcp_method(sockfd);
	
	return 0;
}