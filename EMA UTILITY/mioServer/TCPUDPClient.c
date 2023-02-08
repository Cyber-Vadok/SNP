#include "Header.h"

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

    int quit = 0;
    int counter = 0;
    
    while (quit == 0){
     	char msg[BUFSIZE] = "";
    	recv(sockfd, msg, BUFSIZE-1, 0);
    	if (strlen(msg) == 0){
    		quit = 1;
    	}
    	else {
    		printf("server reply %d: %s\n", counter,msg);
    		counter++;
    		sleep(1);
    	}
    }
    
    close(sockfd);
}

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del client)
int udp_method(int sockfd,struct socklen_t *len, struct sockaddr_in server){
	char msg[BUFSIZE] = "Hello Server!";
	int counter = 0;
	
	sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&server, len);
	while (counter < 5){
		recvfrom(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&server, &len);
		if (strlen(msg) == 0){
			exit(0);
		}else {
			printf("%s %d\n",msg,counter);
			counter++;
			sleep(1);
		}
	}
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
	udp_method(sockfd,len,server);
	
	exit(0);
}


int main(int argc, char *argv[]){
	
	if (argc == 1){
	printf("Inserisci come argomento TCP se vuoi lavorare con TCP oppure UDP se vuoi lavorare con UDP\n");
	printf("Successivamente inserisci l'indirizzo IP della macchina dove ti vuoi connettere\n");
	printf("Se non specifichi un indirizzo ip verrà utilizzato quello di local host\n");
	return 0;
	}
	
	if (strcmp(argv[1],"TCP") == 0){
	int sockfd = tcp_client(argc,argv,PORTTCP);
	tcp_method(sockfd);
	}
	
	if (strcmp(argv[1],"UDP") == 0){
	udp_client(argc,argv,PORTUDP);
	}
	
	return 0;
}