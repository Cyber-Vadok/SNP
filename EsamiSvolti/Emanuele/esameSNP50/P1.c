/*
Esercizio

Siano T1 e T2 due thread di un processo P1 su una macchina A e P2 e
P3 siano due processi sulla macchina B. Lascia che i seguenti eventi accadano in
ordine così definito:

1) T1 genera una stringa casuale e la invia a P2, 2) P2 condivide
la stringa con P3 attraverso una memoria condivisa 3) quindi P3 invia la stringa
to T2 4) tornare al passo 1 non appena T2 ha ricevuto indietro la stringa.

Assicurati che ogni passaggio venga eseguito solo quando lo è il precedente
completato. Avere P1 stampare sullo standard output il tempo con nanosecondi
precisione sul completamento di ogni ciclo.

Per accelerare lo sviluppo, i tre processi possono essere eseguiti sulla stessa macchina
e P1 può comunicare con P2 e P3 attraverso l'indirizzo di loopback.
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
#include <pthread.h>

#define PORTUDP 49152
#define BUFSIZE 256
#define BACKLOG 10

pthread_mutex_t count_mutex;


char *randstring(size_t length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";        
    char *randomString = NULL;

    if (length) {
        randomString = malloc(sizeof(char) * (length +1));

        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}


void t1(size_t length,int sockfd,struct socklen_t *len,struct sockaddr_in client){
	pthread_mutex_lock(&count_mutex);
	char randomString[BUFSIZE] = "";
	strcpy(randomString,randstring(length));
	printf("Invio a P2 da T1 la stringa: %s \n",randomString);
	sendto(sockfd,randomString,strlen(randomString), 0, (struct sockaddr *)&client, len); 
	pthread_mutex_unlock(&count_mutex);
}

int t2(int sockfd,struct socklen_t *len,struct sockaddr_in client){
	pthread_mutex_lock(&count_mutex);
	char mystring[BUFSIZE] = "";
	recvfrom(sockfd, mystring, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
	printf("Ricevo su T2 da P3 la stringa: %s \n",mystring);
	pthread_mutex_unlock(&count_mutex);
	return 0;
}


//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del server)
int udp_method(int sockfd,struct socklen_t *len){

	char buffer[BUFSIZE] = "";
	struct sockaddr_in client;
	char address[INET_ADDRSTRLEN] = "";
	char path[1024] = "/Users/emanuelemele/Desktop";
	
	int quit = 1;
	while (quit == 1){
		recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
		printf("Ricevuto messaggio:'%s' da: %s:%d\n", buffer, inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN), ntohs(client.sin_port) );
		t1(10,sockfd,len,client);
		t2(sockfd,len,client);
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
	printf("\n\tServer UDP listening on port %d\n\n", (int)port);
	
	//Richiamo del metodo scritto da noi
	udp_method(sockfd,len);
	
	exit(0);
}


int main(int argc, char *argv[]){
	

	int sockfd = udp_server(PORTUDP);
    return(0);
}