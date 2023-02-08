#include "Header.h" 

#define IP_ADDR "127.0.0.1"


//dato un socket, stampa porta e indirizzo ip di locale e del server
void printSocketPair(int sockfd){
	
	int res = 0;
	struct sockaddr_storage local;
	struct sockaddr_storage remote;
	socklen_t locallen = sizeof(struct sockaddr_storage);
	socklen_t remotelen = sizeof(struct sockaddr_storage);
	
	res = getsockname(sockfd, (struct sockaddr *)&local, &locallen);
	if (res == -1 ){
		perror("getsockname error:");
	}
	
	res = getpeername(sockfd, (struct sockaddr *)&remote, &remotelen);
	if (res == -1 ){
		perror("getpeername error:");
	}

	char localIP[INET6_ADDRSTRLEN] = ""; //stringa di indirizzo IP del client
	char remoteIP[INET6_ADDRSTRLEN] = ""; //stringa di indirizzo IP del server
	char plocal[PORT_STRLEN] = ""; //stringa di porta TCP del client
	char premote[PORT_STRLEN] = ""; //stringa di porta TCP del server

	//no reverse lookup
	int niflags = NI_NUMERICSERV | NI_NUMERICHOST;
	
	res = getnameinfo((struct sockaddr *)(&local), locallen, localIP, INET6_ADDRSTRLEN, plocal, PORT_STRLEN, niflags);
	
	if (res == 0)
	{
		printf("***** TCP Connection from '%s:%s' ", localIP, plocal);
	}
	else
	{
		printf("getnameinfo() error: %s\n", gai_strerror(res));
	}
	
	res = getnameinfo((struct sockaddr *)(&remote), remotelen, remoteIP, INET6_ADDRSTRLEN, premote, PORT_STRLEN, niflags);
	
	if (res == 0)
	{
		printf("to '%s:%s' ", remoteIP, premote);
	}
	else
	{
		printf("getnameinfo() error: %s\n", gai_strerror(res));
	}
	
printf("*****\n\n");

}


int main(int argc, char *argv[]){

	//---------------------------------------------------------------------------------------------------
	
	struct addrinfo hints, *result, *ptr;
	memset(&hints, 0, sizeof(hints)); //azzeramento della struttura
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_protocol = IPPROTO_TCP; 
	hints.ai_flags |= AI_NUMERICSERV; //non necessaria la risoluzione del service name
	hints.ai_flags |= AI_CANONNAME; //richiediamo il nome canonico

	
	int status = 0;    
	status = getaddrinfo(IP_ADDR, SERVICEPORT, &hints, &result);
	if (status != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return FAILURE;
	}//fi


	int sockfd = 0;
	
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
	{
		sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol); 
		if (sockfd == -1)
			continue; //passo al prossimo risultato
		
		//avvio il 3WHS
		if (connect(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1)
		{
			//se fallisce, devo chiudere il socket e aprirne un altro 
			close(sockfd);
			printf("client connecting in the %s domain error\n", (ptr->ai_family == AF_INET) ? "AF_INET": "AF_INET6");
			perror("connect error:");
			continue;
		}//fi connect

		break;	//se sono qui, sono connesso 

	}//for

	if (ptr == NULL) //nessun address è stato valido per la connessione
	{
		fprintf(stderr, "Client failed to connect\n");
		return FAILURE;
	}


	freeaddrinfo(result); //qui sono connesso e la linked list non serve piu
	
	printSocketPair(sockfd); //visualizzo i dati della connessione


	//---------------------------------------------------------------------------------------------------

	
	
	char buf[BUFSIZE] = "";
	ssize_t numbytes = 0;
	
	int quit = 0;
	while(quit == 0)
	{
	
		fgets(buf, BUFSIZE, stdin); //prendi string da inviare da stdin
		buf[strlen(buf) - 1] = 0;
		
		//SE IL CLIENT SCRIVE QUIT SMETTE DI MANDARE MESSAGGI
		if (strcmp("quit", buf) == 0)
		{
			quit = 1;
			break; //metto il break a mano altrimenti invia il messaggio quit
		}
		
		
		numbytes = send(sockfd, buf, strlen(buf), 0);
		if (numbytes == -1)
		{
			perror("send(): ");
			close(sockfd);
			return FAILURE;
		}
		

		numbytes = recv(sockfd, buf, BUFSIZE, 0);
		if (numbytes > 0)
		{
			buf[numbytes] = '\0';
			printf("Received: %s\n", buf);
		}
		
		
		
		
	}
	close(sockfd);	//chiudiamo la connessione

	
return 0;
}

