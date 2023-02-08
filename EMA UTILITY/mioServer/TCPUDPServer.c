#include "Header.h"

//Creazione del server TCP
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
		//mi ricavo l'indirizzo di chi si è connesso in notazione decimale puntata
		char clientaddr[INET_ADDRSTRLEN] = "";
		inet_ntop(AF_INET, &(peer_addr->sin_addr), clientaddr, INET_ADDRSTRLEN);
		printf("\tAccepted a new TCP connection from %s:%d\n", clientaddr, ntohs(peer_addr->sin_port));
		
		int count = 0; //numero di cicli che voglio effettuare
		char buf[BUFSIZE] = "Hello World!";
		
		while (count < 5){
			send(peerfd, buf, strlen(buf), 0);
			count++;
			sleep(1);
		}
		printf("\tChild %d terminating..\n", (int)getpid());
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

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del server)
int udp_method(int sockfd,struct socklen_t *len){

	char buffer[BUFSIZE] = "Hello World!";
	struct sockaddr_in client;
	char address[INET_ADDRSTRLEN] = "";
	int quit = 1;
	while (quit == 1){
		int counter = 0;
		recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
		printf("\tRicevuto messaggio:'%s' da: %s:%d\n", buffer, inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN), ntohs(client.sin_port) );
		strncpy(buffer, "Hello Client!", sizeof(buffer));
		while(counter < 5){
			sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client, len);
			counter++;
			sleep(1);
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
	printf("\n\tServer UDP listening on port %d\n", (int)port);
	
	//Richiamo del metodo scritto da noi
	udp_method(sockfd,len);
	
	return sockfd;
}


int main(int argc, char *argv[]){

	if (argc == 1){
	printf("Inserisci come argomento TCP se vuoi lavorare con TCP oppure UDP se vuoi lavorare con UDP\n");
	return 0;
	}
	
	
	if (strcmp(argv[1],"TCP") == 0){
		int sockfd = tcp_server(PORTTCP);
		tcp_concurrent(sockfd);	
	}
	
	if (strcmp(argv[1],"UDP") == 0){
		int sockfd = udp_server(PORTUDP);
	}
	
	
	
	
	return 0;
}