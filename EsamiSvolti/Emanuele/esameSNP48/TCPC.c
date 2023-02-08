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

#define BUFSIZE 256
#define BACKLOG 10

//Creazione del client TCP
int tcp_client(int argc, char *argv[],int port){

	int sockfd = 0; //connection socket: servirà per la comunicazione col server
	
	struct sockaddr_in server; //IPv4 server address, senza hostname resolution 
	socklen_t len = sizeof(server); //dimensione della struttura di indirizzo
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address FAmily IPv4
	server.sin_port = htons(port);

	if (argc < 3){
		server.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //se voglio connettermi in localhost
	}else {
		inet_pton(AF_INET, argv[2], &(server.sin_addr)); //altrimenti salvo l'indirizzo ip passato come parametro in notazione decimale puntata
	}
	
	
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); //apro il socket del client
    connect(sockfd, (struct sockaddr *)&server, len); //mi connetto al server
    return sockfd;
    
}

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del client)
void tcp_method(int sockfd,char *argv[],int argc){

    char buf[BUFSIZE] = "";
    
    if (argc == 3){
    	strcpy(buf,argv[2]);
    }
    if (argc == 4){
    	strcpy(buf,argv[3]);
    }
    
    send(sockfd, buf, strlen(buf), 0);
    
    printf("Messaggio inviato!\n");
    recv(sockfd, buf, BUFSIZE-1, 0);
    printf("%s\n",buf);
    
    close(sockfd);
}

int main(int argc, char *argv[]){

	//Eseguire il programma con ./TCPC.c P1 "Stringa"
	int PORTTCP = 0;
	
	if (strcmp(argv[1],"P1") == 0){
		PORTTCP = 49152;
	}
	if (strcmp(argv[1],"P2") == 0){
		PORTTCP = 49153;
	}
	
	int sockfd = tcp_client(argc,argv,PORTTCP);
	tcp_method(sockfd,argv,argc);
	
	return 0;
}