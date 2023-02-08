#include "Header.h"

/*
void sigchld_handler(int s){
	while(waitpid(-1, NULL, WNOHANG) > 0);
}
*/

/*
void handleClientConnection(int clientfd){
	
	//-------------------------------------------------------------------------------------------------
	// ORA SI FA PRIMA GETPEERNAME PER AVERE INFORMAZIONI DEL CLIENT, E POI SI PASSA LA STRUTTURA SOCKADDR A GETNAMEINFO PER AVERE IP E PORTA COME STRINGA
	struct sockaddr_in client; //struttura per salvare informazioni del client
	socklen_t sslen = sizeof(struct sockaddr_storage);
	
	int res = 0;
	res = getpeername(clientfd, (struct sockaddr *)&client, &sslen); //getpeername per estrare il remote address dal socket connesso. 
	if (res == -1)
	{
		perror("getpeername error: ");
		exit(1);
	}
	
	char ip[INET6_ADDRSTRLEN] = "";//stringa di indirizzo IP del client
	char port[PORT_STRLEN] = "";//stringa di porta TCP del client
	res = getnameinfo((struct sockaddr *)(&client), sslen, ip, INET6_ADDRSTRLEN, port, PORT_STRLEN, NI_NUMERICSERV | NI_NUMERICHOST); //partendo dalla struttura sockaddr, salvo dentro le stringhe la porta e l'ip del client
	if (res == 0)
	{
		printf("\nConnection with client from '%s:%s' ", ip, port);
	}
	else
	{
		printf("getnameinfo() error: %s\n", gai_strerror(res));
	}
	

	//-------------------------------------------------------------------------------------------------

	char msg[BUFSIZE] = "";
	ssize_t num = 0;
	
	int quit = 0;
	while (!quit)
	{	
		num = recv(clientfd, &msg, BUFSIZE - 1, 0); 
		
		if (num == -1)
		{
			perror("Recv error: ");
			close(clientfd);
			exit(1);
		}else if(num == 0)
		{
			//printf("client closed the connection\n");
			quit = 1;
		} else {
			msg[num] = '\0';
			printf("Received %d bytes msg: '%s'\n", (int)num, msg);
			
			num = send(clientfd, msg, strlen(msg), 0);
			if (num == - 1){
				perror("Send failed: ");
			}
		}//fi
	}//wend
	
	//il client ha chiuso la connessione, dealloco il socket
	close(clientfd);
}
*/


void * handleClientConnection(void* arg){

	int clientfd = (int) arg;
	
	/*
	//-------------------------------------------------------------------------------------------------
	// ORA SI FA PRIMA GETPEERNAME PER AVERE INFORMAZIONI DEL CLIENT, E POI SI PASSA LA STRUTTURA SOCKADDR A GETNAMEINFO PER AVERE IP E PORTA COME STRINGA
	struct sockaddr_in client; //struttura per salvare informazioni del client
	socklen_t sslen = sizeof(struct sockaddr_storage);
	
	int res = 0;
	res = getpeername(clientfd, (struct sockaddr *)&client, &sslen); //getpeername per estrare il remote address dal socket connesso. 
	if (res == -1)
	{
		perror("getpeername error: ");
		exit(1);
	}
	
	char ip[INET6_ADDRSTRLEN] = "";//stringa di indirizzo IP del client
	char port[PORT_STRLEN] = "";//stringa di porta TCP del client
	res = getnameinfo((struct sockaddr *)(&client), sslen, ip, INET6_ADDRSTRLEN, port, PORT_STRLEN, NI_NUMERICSERV | NI_NUMERICHOST); //partendo dalla struttura sockaddr, salvo dentro le stringhe la porta e l'ip del client
	if (res == 0)
	{
		printf("\nConnection with client from '%s:%s'\n", ip, port);
	}
	else
	{
		printf("getnameinfo() error: %s\n", gai_strerror(res));
	}
	*/

	//-------------------------------------------------------------------------------------------------

	char msg[BUFSIZE] = "";
	ssize_t num = 0;
	
	int quit = 0;
	while (!quit)
	{	
		num = recv(clientfd, &msg, BUFSIZE - 1, 0); 
		
		if (num == -1)
		{
			perror("Recv error: ");
			close(clientfd);
			exit(1);
		}else if(num == 0)
		{
			printf("client closed the connection\n");
			quit = 1;
		} else {
			msg[num] = '\0';
			printf("Received %d bytes msg: '%s'\n", (int)num, msg);
			
			num = send(clientfd, msg, strlen(msg), 0);
			if (num == - 1){
				perror("Send failed: ");
			}
		}//fi
	}//wend
	
	//il client ha chiuso la connessione, dealloco il socket
	close(clientfd);
	return (void*) NULL;
}

int main(int argc, char *argv[]){
	
	/*
	struct sigaction sa; //signal handler per la gestione dei child
	sa.sa_handler = sigchld_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; //fa ripartire la funzione che si è fermata una volta che si torna dal signal handler
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) 
    {
    	perror("sigaction error: ");
		return -1;
	}
	*/
	
	//-------------------------------------------------------
	
	int sockfd = 0; //listening socket 
	int ecode = 0; //per valore di ritorno delle system call invocate
	
	struct addrinfo hints, *result, *p;
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; //TCP
	hints.ai_flags |= AI_PASSIVE; //per disabilitare service name resolution 
	hints.ai_flags |= AI_NUMERICSERV;
	
	ecode = getaddrinfo(NULL, SERVICEPORT, &hints, &result); //invoco il resolver
	if (ecode != 0)
	{
		printf("getaddrinfo: %s\n", gai_strerror(ecode));
		return ecode;
	}
	
	for (p = result; p != NULL; p = p->ai_next)	//loop through the linked list until success
	{
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);		//crea il socket
		if (sockfd == -1)
		{	
			perror("socket() error: ");
			continue;	//skip to next
		}
				
		ecode = bind(sockfd, p->ai_addr, p->ai_addrlen); //assegna l'indirizzo risolto

		if (ecode == -1)
		{
			perror("bind() error: ");
			close(sockfd);
			continue; //skip to next
		}
			
		break; //success
	}//for
	
	if (p == NULL) 
	{
		printf("Abbiamo testato tutte le strutture di indirizzo ma non siamo riusciti ad aprire un socket ed assegnargli l'indirizzo desiderato nel dominio AF_INET%s\n", argv[1]);
		return FAILURE;
	}//fi NULL
	
	
	freeaddrinfo(result);	//libero le risorse di getaddrinfo(): non mi servono più
	
	ecode = listen(sockfd, BACKLOG); //mi metto in ascolto
	if (ecode == -1)
	{
		perror("listen() error: ");
		close(sockfd);
		return FAILURE;
	}
	
	//-------------------------------------------------------
	
	//PER SALVARE LE INFORMAZIONI LOCALI DEL SERVER
	/*
	struct sockaddr_in local; //salvo le informazioni del server stesso 
	socklen_t len = sizeof(local);
	ecode = getsockname(sockfd, (struct sockaddr *)&local, &len); //tramite getsockname ottengo le informazioni "locali", quindi del server in questo caso
	if (ecode == -1 ){u
		perror("getsockname on listening socket ERROR:");
		return FAILURE;
	}
	
	printf("\tTCP Server listening on port %s, domain %s\n\n", SERVICEPORT, (local.ss_family == AF_INET6) ? "IPv6": "IPv4");
	*/
	
	int client_fd = 0; //PER OGNI CONNESSIONE CON UN CLIENT, SI APRE UN FD

	pthread_t t1;
	while (1)
	{

		client_fd = accept(sockfd, NULL, NULL); //prelevo connessione dalla coda, e nota che non importa l'indirizzo del client
		if (client_fd == -1)
		{
			perror("accept() error: ");
			continue; 
		}

		
		/*
		//PER UTILIZZARE UN CHILD PER OGNI CONNESSIONE
		RICORDATI DI TOGLIERE I COMMENTI ALLA GESTIONE DI SIGCHILD ALL'INIZIO DEL MAIN
		E ANCHE ALLA FUNZIONE USATA COME GESTORE. 
		if ( fork() == 0) //creo un child per ogni connessione 
		{
			close(sockfd); //NEL CHILD POSSO CHIUDERE IL LISTENING SOCKET
			handleClientConnection(client_fd);
			exit(0);
		}

		close(client_fd); //AL parent non interessa gestire il client
		*/
		
		if (pthread_create(&t1, NULL, handleClientConnection, (void*) client_fd) != 0)
    		printf("Errore nella creazione della thread");    
        
	}//wend

	close(sockfd);

return 0;
}
