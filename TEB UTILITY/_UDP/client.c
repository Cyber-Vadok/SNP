#include "Header.h"

#define SERVER_IP "127.0.0.1"


void printAddressInfo(struct sockaddr * addr, socklen_t len){ //prendendo la struttura sockaddr, stampa ip e porta passando tramite la funzione getnameinfo 

	//no reverse lookup in getnameinfo
	int niflags = NI_NUMERICSERV | NI_NUMERICHOST;
	char IP[INET6_ADDRSTRLEN] = "";
	char port[PORT_STRLEN] = "";
	
	//visualizzo l'indirizzo locale del socket
	int rv = getnameinfo(addr, len, IP, INET6_ADDRSTRLEN, port, PORT_STRLEN, niflags);
	
	if (rv == 0)
	{
		printf("'%s:%s\n'", IP, port);
	}
	else
	{
		printf("getnameinfo() error: %s\n", gai_strerror(rv));
	}
}


int connectedSocket = 0;

int main(int argc, char *argv[]) {
	
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; //siamo nel dominio IPv4
	hints.ai_socktype = SOCK_DGRAM; //comunicazione UDP

	if ((rv = getaddrinfo(SERVER_IP, SERVICEPORT, &hints, &servinfo)) != 0) //ottengo informazioni del server basandomi sui suggerimenti 

	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return FAILURE;
	}

	//loop su tutti i risultati, creando un socket per il primo con cui riesco con successo
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
		{
			perror("UDP client socket() error: ");
			continue;
		}

		break;
	}//for

	if (p == NULL) 
	{
		fprintf(stderr, "%s: failed to open socket\n", argv[0]);
		return FAILURE;
	}
	
			
	
	ssize_t numbytes; //numero byte inviati/ricevuti 
	char buf[BUFSIZE]; //dove viene memorizzato il messaggio
	char IP[INET6_ADDRSTRLEN]; //dove viene salvato l'indirizzo IP
	
	if (connectedSocket == 1)
	{
		//eseguiamo la connect sul socket descriptor...
		rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
		if (rv != 0 )
		{
			perror("UDP connect() error: ");
			close(sockfd);
			return FAILURE;
		}//fi

		//quindi posso liberare la linked-list...
		freeaddrinfo(servinfo);
	
		//LA STRUTTURA LOCAL SERVE PER SALVARE LE INFORMAZIONI SU DI NOI (CLIENT) O SUL SERVER
		/*
		struct sockaddr_in local;
		socklen_t locallen = sizeof(struct sockaddr_in);
		memset(&local, 0, locallen);
		*/
		
		//ALLO SCOPO DI SAPERE IL PROPRIO IP E LA PROPRIA PORTA 
		/*
		rv = getsockname(sockfd, (struct sockaddr *)&local, &locallen); //riempie la struttura local
		if (rv < 0)
		{
			perror("error on getsockname: ");
			close(sockfd);
			return FAILURE;
		}
		
		printf("Local UDP Address: ");
		printAddressInfo((struct sockaddr *)&local, locallen);
		*/
		
		
		numbytes = send(sockfd, argv[1], strlen(argv[1]), 0); //ora posso eseguire la send invece di sendto
		if (numbytes == -1)
		{
			perror("UDP send() error: ");
			close(sockfd);
			return FAILURE;
		}
		printf("UDP client: sent %d bytes\n", (int)numbytes);
	
	
		//ALLO SCOPO DI SAPERE IP E PORTA DI SERVER 
		/*
		rv = getpeername(sockfd, (struct sockaddr *)&local, &locallen);
		if (rv < 0)
		{
			perror("error on getpeername: ");
		}
		printf("Connected to remote peer address ");
		printAddressInfo((struct sockaddr *)&local, locallen);
		*/
		
		numbytes = recv(sockfd, buf, BUFSIZE-1, 0);
		if (numbytes == -1)
		{
			perror("UDP recv() error: ");
			close(sockfd);
			return FAILURE;
		}
		else
		{
			buf[numbytes] = 0;
			printf("Received %d bytes Message '%s'\n", (int)numbytes, buf);
		}
	}
	else //SOCKET NON CONNESSO 
	{ 
		numbytes = sendto(sockfd, argv[1], strlen(argv[1]), 0, p->ai_addr, p->ai_addrlen);
		if (numbytes == -1) 
		{
			perror("udp sendto() error: ");
			close(sockfd);
			return FAILURE;
		}

		
		freeaddrinfo(servinfo);	 //POSSO LIBERARE SERVINFO SOLO QUI PERCHE NELLA SENDTO PRELEVO L'INDIRIZZO IP DA QUI

		struct sockaddr_storage peer;
		socklen_t len = sizeof(struct sockaddr_storage);
		memset(&peer, 0, len);
		
		//PRENDE INFORMAZIONI SUL CLIENT
		/*
		//con un socket non connesso possiamo leggere i dati locali del socket dopo l'invio
		rv = getsockname(sockfd, (struct sockaddr *)&peer, &len);
		if (rv < 0)
		{
			perror("error on getsockname: ");
			close(sockfd);
			return FAILURE;
		}
		printf("Local UDP Address: ");
		printAddressInfo((struct sockaddr *)&peer, len);
		*/

		numbytes = recvfrom(sockfd, buf, BUFSIZE-1, 0, (struct sockaddr *)&peer, &len); //chiamata bloccante
		if (numbytes == -1) 
		{
			perror("udp recvfrom() error: ");
			close(sockfd);
			return FAILURE;
		}
		else
		{
			buf[numbytes]=0;
			printf("\nReceived '%s' ", buf);
			//printf("from: ")
			//printAddressInfo((struct sockaddr *)&peer, len); //visualizzo l'indirizzo del peer che mi ha contattato

		}

	}//fi connect
	printf("\nClosing socket\n");
	close(sockfd);

return 0;
}

/**@}*/