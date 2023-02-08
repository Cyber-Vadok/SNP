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

int speed;

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
		
		char buf[BUFSIZE] = "";
		int counter = 1;
		while(1){
			recv(peerfd, buf, BUFSIZE-1, 0);
			printf("comando arrivato: %s\n",buf);

			if(strcmp(buf,"u") ==  0){
				if (speed != 0){
					speed = speed - 1;
					printf("speed up\n");
				}
			}
			if(strcmp(buf,"d") ==  0){
				speed = speed + 1;
				printf("speed down\n");
			}
			if(strcmp(buf,"s") ==  0){
				send(peerfd, "stop", strlen("stop"), 0);
				exit(0);
			}
			sleep(speed);
			strcpy(buf,"message");

			char scount[30];
			sprintf(scount, "%d", counter);
			
			strcpy(buf,scount);

			send(peerfd, buf, strlen(buf), 0);
			counter++;
			strcpy(buf," ");
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

int main(int argc, char *argv[]){

	speed = atoi(argv[1]);
	int sockfd = tcp_server(PORTTCP);
	tcp_concurrent(sockfd);	

	return 0;
}