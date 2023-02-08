#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTUDP 49152
#define BUFSIZE 256
#define BACKLOG 10

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del client)
int udp_method(int sockfd,struct socklen_t *len, struct sockaddr_in server,char *argv[]){
	char msg[BUFSIZE] = "Ciao Server !";
	
	sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&server, len);
	strncpy(msg, "", sizeof(msg));
	
	
	recvfrom(sockfd, msg, BUFSIZE-1, 0, (struct sockaddr *)&server, &len);
	printf("%s \n",msg);	
			
	printf("\nExit\n");
}

//Creazione del client UDP
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
	udp_method(sockfd,len,server,argv);
	
	exit(0);
}


int main(int argc, char *argv[]){
	
	udp_client(argc,argv,PORTUDP);
	
	return 0;
}