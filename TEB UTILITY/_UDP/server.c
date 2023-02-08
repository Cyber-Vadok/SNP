#include "Header.h"

int main(int argc, char *argv[]) {
	
	int status = 0; // valore di ritorno delle Sockets API

	struct addrinfo hints, *result, *ptr; //parametri in input al resolver 
	memset(&hints, 0, sizeof(hints)); //azzeramento della struttura

	hints.ai_family = AF_INET; //IPv4
	hints.ai_flags = AI_PASSIVE; //server side resolution 
	hints.ai_socktype = SOCK_DGRAM; //UDP
	hints.ai_flags |= AI_NUMERICSERV;  //non passo un service name ma una porta

	status = getaddrinfo(NULL, SERVICEPORT, &hints, &result); //utilizzo gli hint per riempire la struttura result con tutte le informazioni
	if (status != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return FAILURE;
	}

	int sockfd = 0; //socket descriptor del server 
	
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) //addrinfo ritorna una double linked list, quindi la esploriamo e creiamo un socket e facciamo bind (con il primo risultato di "successo"). 
	{ 
		sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol); //apro un socket

		if (sockfd == -1)
			continue; //passo al prossimo elemento della lista
			
		if (bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) == 0) //faccio la bind
			break; // Success
		
		close(sockfd); //se sono qui la bind non è andata a buon fine, allora chiudo il sockfd e provo ad aprirne un altro iterando nel loop
	}//for

	if (ptr == NULL) 
	{ 
		fprintf(stderr, "Could not bind\n");
		return FAILURE;
	}

	
	freeaddrinfo(result); //dealloco le risorse usate da getaddrinfo

	
	printf("UDP Echo Server waiting messages on port %s ...\n", SERVICEPORT); //il server UDP è pronto per ricevere messaggi
	
	
	char msg[BUFSIZE] = ""; //buffer che conterrà i datagrammi ricevuti 
	ssize_t numbytes = 0; //dimensione del datagramma ricevuto 
	
	//char peerIP[INET6_ADDRSTRLEN] = ""; // stringa per memorizzare indirizzo IP del client 
	//char peerPort[PORT_STRLEN] = ""; //stringa per rapprensentazione porta UDP del client 
	struct sockaddr_in peerAddr; // struttura che memorizza ip, porta e famiglia del client
	socklen_t len = sizeof(struct sockaddr_in); // dimensione della struttura di indirizzo generica 

	
	int quit = 0; // condizione di iterazione del server 

	
	while(!quit) //ciclo basato sul valore di quit 
	{
		numbytes = recvfrom(sockfd, msg, BUFSIZE-1, 0, (struct sockaddr *)(&peerAddr), &len); //recvfrom è bloccante, e ritorna quando riceve qualcosa sul socketfd
		
		if (numbytes == -1) //se la recvfrom fallisce
		{
			perror("UDP Server recvfrom error: ");
			quit = 1;
			return FAILURE;
		}
		
		//PER SALVARE IP E PORTA DEL CLIENT
		/*
		//visualizzo l'indirizzo del peer che mi ha contattato:
		status = getnameinfo((struct sockaddr *)(&peerAddr), len, peerIP, INET6_ADDRSTRLEN, peerPort, PORT_STRLEN, NI_NUMERICSERV | NI_NUMERICHOST);
		
		if (status == 0)
		{
			printf("***** Received message from  '%s:%s' ", peerIP, peerPort);
		}
		else
		{
			printf("getnameinfo() error: %s\n", gai_strerror(status));
		}
		*/
		
		msg[numbytes] = '\0'; //faccio terminare il messaggio del client salvato in msg con null
		printf("Message (len = %d): '%s'\n", (int)numbytes, msg);
		
		numbytes = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)(&peerAddr), len); //invio del datagram contenente la risposta per il client

		if (numbytes == -1)
		{
			perror("UDP Server sendto error: ");
			quit = 1;
			return FAILURE;
		}	
	}//wend
		
	close(sockfd); //il server non è più in ascolto, chiudo il file descriptor

	
return 0;
}
	