
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
#include <pthread.h>
#include <fcntl.h>


#define PORTTCP 49152
#define BUFSIZE 256
#define BACKLOG 10
#define MAXHOST 256

pthread_t tid[MAXHOST]; //con 256 numero di host massimi
int tid_control[MAXHOST]; //assegna le thread
pthread_mutex_t lock;

struct parameters_for_thread {
    int peerfd;
    struct sockaddr_in *peer_addr;
	int my_tid_control;
};

int tcp_server(int port){

	//inizializzo il contatore per le thread
	for (int i = 0; i < MAXHOST; i++){
		tid_control[i] = 0;
	}


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
int tcp_method(void *params){

		//faccio il cast della struttura per prendermi i parametri
		struct parameters_for_thread *p = (struct parameters_for_thread *) params;
		int peerfd = p->peerfd;
		struct sockaddr_in *peer_addr = p->peer_addr;
		int my_tid_control = p->my_tid_control;

		
		char buf[BUFSIZE] = "";
		
		//mi ricavo l'indirizzo di chi si è connesso in notazione decimale puntata
		char clientaddr[INET_ADDRSTRLEN] = "";
		inet_ntop(AF_INET, &(peer_addr->sin_addr), clientaddr, INET_ADDRSTRLEN);
		printf("Accepted a new TCP connection from %s:%d\n", clientaddr, ntohs(peer_addr->sin_port));
		
		pthread_mutex_lock(&lock); //prendo il mutex
		recv(peerfd, buf, BUFSIZE-1, 0);	
		printf("Ho ricevuto: %s\n",buf);
		send(peerfd, "Stop", strlen("Stop"), 0);
		sleep(5);
		printf("Thread terminating..\n\n");
		pthread_mutex_unlock(&lock); //rilascio il mutex

		//chiusura della thread
		tid_control[my_tid_control] = 0;
		return 0;
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

		for (int i = 0; i < MAXHOST; i++)
		{
			if (tid_control[i] == 0)
			{
				struct parameters_for_thread p = {peerfd, &peer_addr, i};
				tid_control[i] = 1;
				pthread_create(&(tid[i]), NULL, &tcp_method, &p); //faccio partire la thread
				break;
			}
			
		}
		
		//pthread_join(tid[i], NULL); //aspetto che finisca la thread
		//close(peerfd);
	}
}

int main(int argc, char *argv[]){
	
	pthread_mutex_init(&lock, NULL); //inizializzo il mutex
	int sockfd = tcp_server(PORTTCP);
	tcp_concurrent(sockfd);	

	return 0;
}