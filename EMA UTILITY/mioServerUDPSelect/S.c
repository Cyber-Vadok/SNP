#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <semaphore.h>

#define PORTUDP 49152
#define BUFSIZE 256
#define BACKLOG 10

fd_set read_fds;
struct timeval timeout;

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del server)
int udp_method(int sockfd,struct socklen_t *len){

	char buffer[BUFSIZE] = "";
	struct sockaddr_in client;
	char address[INET_ADDRSTRLEN] = "";

	// Utilizzo della select() per controllare se il client ha scritto al server
	int ready = select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);
	if (ready < 0) {
		// Errore
	} else if (ready == 0) {
		printf("Tempo di ricezione scaduto! \n");
	} else {
		if (FD_ISSET(sockfd, &read_fds)) {
			// Il client ha scritto al server, elaborazione dei dati
			recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
			printf("Ricevuto messaggio:'%s' da: %s:%d\n", buffer, inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN), ntohs(client.sin_port) );
			printf("Invio il messaggio di chiusura....\n");
			sendto(sockfd,"Stop",strlen("stop"), 0, (struct sockaddr *)&client, len); //messaggio finale di chiusura
			// ...
		}
	}
	
	
	return 0;
}

//Creazione del server UDP
int udp_server(int port){
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);

	// Impostazione del timeout
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

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

	int sockfd = udp_server(PORTUDP);
    return(0);
}