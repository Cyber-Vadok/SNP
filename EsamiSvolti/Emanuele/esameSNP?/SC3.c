/*
Tre processi si scambiano ciclicamente tra loro, il più velocemente possibile,
un datagramma come se fosse una palla. Misurare e confrontare la velocità di
rotazione della palla nel caso di processi ospitati dalla stessa macchina e nel
caso di processi ospitati in due macchine differenti. Accertarsi, con qualunque
metodo, che in ogni istante ci sia al massimo un datagramma in circolazione nel
sistema costituito dai tre processi.
*/

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
#include <semaphore.h>

#define PORTUDPCLIENT 49152
#define PORTUDPSERVER 49153
#define BUFSIZE 256
#define BACKLOG 10

int udp_method_server(int sockfd,struct socklen_t *len){

	char buffer[BUFSIZE] = "";
	struct sockaddr_in client;
	char address[INET_ADDRSTRLEN] = "";
	
	recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
	printf("Ricevuto messaggio:'%s' da: %s:%d\n", buffer, inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN), ntohs(client.sin_port) );
		
	return 0;
}

int udp_method_client(int sockfd,struct socklen_t *len, struct sockaddr_in server,char *argv[]){
	char msg[BUFSIZE] = "Datagram SC3 in viaggio...";
	
	sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&server, len);
	strncpy(msg, "", sizeof(msg));
	printf("Il clinet 1 ha inviato il datagram..\n");
	printf("Exit client\n");

}

int udp_client(int argc, char *argv[],int port){
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in server;
	socklen_t len = sizeof(server);
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address FAmily IPv4
	//Specifico l'indirizzo del server inet_pton()
	if (argc < 2){
		server.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //se voglio connettermi in localhost
	}else {
		inet_pton(AF_INET, argv[1], &(server.sin_addr)); //altrimenti salvo l'indirizzo ip passato come parametro in notazione decimale puntata
	}
	server.sin_port = htons(port);
	printf("\nClient start\n");
	udp_method_client(sockfd,len,server,argv);
	close(sockfd);
	return 0;
}

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
	printf("\nServer UDP listening on port %d\n", (int)port);
	
	//Richiamo del metodo scritto da noi
	udp_method_server(sockfd,len);
	close(sockfd);
	return 0;
}

int main(int argc, char *argv[]){

	int counter = 1;
	while (1){
		int sockfd = udp_server(PORTUDPSERVER);
		udp_client(argc,argv,PORTUDPCLIENT);
		counter++;
	}
    
	return(0);
}