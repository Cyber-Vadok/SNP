
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
		printf("Accepted a new TCP connection from %s:%d\n\n", clientaddr, ntohs(peer_addr->sin_port));
		

		recv(peerfd, buf, BUFSIZE-1, 0);	
		printf("Ho ricevuto: %s\n",buf);
		send(peerfd, "Stop", strlen("Stop"), 0);

		printf("Child %d terminating..\n\n", (int)getpid());
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
		if (fork() == 0){
			tcp_method(peerfd, &peer_addr);
		}
		close(peerfd);
	}
}

int main(int argc, char *argv[]){
	
	int sockfd = tcp_server(PORTTCP);
	tcp_concurrent(sockfd);	

	return 0;
}