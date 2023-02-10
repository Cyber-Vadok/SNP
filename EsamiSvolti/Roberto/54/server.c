#include "Header.h"
#include <sys/mman.h>
#include <fcntl.h>
#define SIZE 400

void usage(char *name)
{
	printf("Usage: %s<domain>\n", name);
	printf("\tdomain=0 => AF_UNSPEC domain\n");
	printf("\tdomain=4 => AF_INET domain\n");
	printf("\tdomain=6 => AF_INET6 domain\n");
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		usage(argv[0]);
		return INVALID;
	}

	/*****************************************************/
	//MEMORIA CONDIVISA POSIX

	void *addr_share;
	int fd_share;

	//creo la memoria condivisa, che ritorna il fd corrispondente
	//fd_share = shm_open("/sharedmemory", O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0666);	//PER LEGGERE TOGLI O_CREAT
	fd_share = shm_open("/sharedmemory", O_RDWR | O_CREAT | O_TRUNC, 0666);	//PER LEGGERE TOGLI O_CREAT
	if (fd_share == -1)
	{
		printf("Errore nel creare la memoria condivisa posix");
		return -1;
	}

	//NOTA: QUESTO FTRUNCAT LO PUOI FARE SOLO UNA VOLTA... quindi potresti dover cambiare chiave ogni run
	//NOTA LO FAI SOLO SU CHI APRE LA MEMORIA
	if (ftruncate(fd_share, SIZE) != 0)
	{
		perror("ftruncate() error");
		printf("DEVI CAMBIARE IDENTIFICATIVO DELLA MEMORIA OPPURE TOGLIERE QUESTA TRUNCATE, CHE PUOI FARE ALLA MEMORIA SOLO UNA VOLTA");
	}

	addr_share = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_share, 0);
	if (addr_share == MAP_FAILED)
	{
		printf("Errore nel mappare la memoria condivisa posix");
		return -1;
	}

	//FINE BLOCCO MEMORIA CONDIVISA
	/*********************************************************************************************/

	/*********************************************************************************************/
	//INIZIO BLOCCO REGION LOCK
	int fd_lock;
	struct flock region_lock;

	//NOTA SE HAI OCREAT E OEXCL E IL FILE ESISTE GIA, LA OPEN FALLISCE
	fd_lock = open("lock_file", O_RDWR | O_CREAT, 0666);
	if (!fd_lock)
	{
		fprintf(stderr, "Unable to open lockfile for read/write\n");
		exit(EXIT_FAILURE);
	}

	//settiamo il lock
	region_lock.l_whence = SEEK_SET;
	region_lock.l_start = 0;
	region_lock.l_len = 8;

	/*********************************************************************************************/

	int status = 0;	// valore di ritorno delle Sockets API

	//parametri in input al resolver 
	struct addrinfo hints, *result, *ptr;
	memset(&hints, 0, sizeof(hints));	//azzeramento della struttura

	//specifico il dominio di comunicazione (e di indirizzi richiesti)
	switch (atoi(argv[1]))
	{
		case 0:
			hints.ai_family = AF_UNSPEC;
			break;
		case 4:
			hints.ai_family = AF_INET;
			break;
		case 6:
			hints.ai_family = AF_INET6;
			break;
	}	//switch

	//server side resolution
	hints.ai_flags = AI_PASSIVE;
	//servizio datagram per il socket 
	hints.ai_socktype = SOCK_DGRAM;
	//non passo un service name ma un valore di porta UDP
	hints.ai_flags |= AI_NUMERICSERV;

	status = getaddrinfo(NULL, SERVICEPORT, &hints, &result);
	if (status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return FAILURE;
	}

	int sockfd = 0; /*socket descriptor per il server UDP */
	//predisponiamo il socket UDP
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		//apro un socket 
		//con i parametri dell'elemento corrente della lista di risultati
		sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sockfd == -1)
			continue;	//passo al prossimo elemento della lista

		if (bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) == 0)
			break;	// Success

		/*
		altrimenti chiudo il socket per cui il bind è fallito
		e passo al prossimo elemento della lista
		*/
		close(sockfd);
	}	//for

	if (ptr == NULL)
	{
		/*No address succeeded */
		fprintf(stderr, "Could not bind\n");
		return FAILURE;
	}

	//dealloco le risorse usate da getaddrinfo
	freeaddrinfo(result);

	//il server UDP è pronto per ricevere messaggi
	printf("UDP Echo Server waiting messages on port %s ...\n", SERVICEPORT);

	/****************************************************************************************************************************************/

	char msg[BUFSIZE] = ""; /*buffer che conterrà i datagra ricevuti */
	ssize_t numbytes = 0;
	char peerIP[INET6_ADDRSTRLEN] = "";	// stringa per memorizzare IPv4/IPv6 addresses 
	char peerPort[PORT_STRLEN] = "";	//stringa per rapprensentazione porta UDP

	// struttura generica per gestire sia IPv4 che IPv6 
	struct sockaddr_storage peerAddr;
	// dimensione della struttura di indirizzo generica 
	socklen_t len = sizeof(struct sockaddr_storage);

	int quit = 0; /*condizione di iterazione del server */

	//no reverse lookup in getnameinfo
	int niflags = NI_NUMERICSERV | NI_NUMERICHOST;

	//ciclo (infinito) basato sul valore di quit - condizione di uscita
	while (!quit)
	{
	        sleep(1);
	        		//lock region
		region_lock.l_type = F_WRLCK;	//si setta a lettura e scrittura
		if (fcntl(fd_lock, F_SETLKW, &region_lock) == -1)
		{
			fprintf(stderr, "Failed to lock region 1\n");
		}
		else
		{
			printf("Process %d locked file\n", getpid());
		}
		/*
		chiamata bloccante:
		ritorna quando c'è almeno un datagram da elaborare, o in caso di errore sul socket.
		*/
		numbytes = recvfrom(sockfd, msg, BUFSIZE - 1, 0, (struct sockaddr *)(&peerAddr), &len);

		if (numbytes == -1)
		{
			perror("UDP Server recvfrom error: ");
			quit = 1;
			//return FAILURE;
		}

		//visualizzo l'indirizzo del peer che mi ha contattato:
		status = getnameinfo((struct sockaddr *)(&peerAddr), len, peerIP, INET6_ADDRSTRLEN, peerPort, PORT_STRLEN, niflags);

		if (status == 0)
		{
			printf("*****Received message from  '%s:%s' ", peerIP, peerPort);
		}
		else
		{
			printf("getnameinfo() error: %s\n", gai_strerror(status));
		}

		//null-terminated string 
		msg[numbytes] = '\0';

		//char stringa_temp[500];
		//sprintf(stringa_temp, "*****Received message from  '%s:%s' \n Message (len = %d): '%s'\n", peerIP, peerPort,(int)numbytes, msg);
		//int lunghezza = strlen(stringa_temp);
		//dprintf(fd_share, "%d\n", lunghezza);


                char messaggio [400];
                sprintf(messaggio, "*****Received message from  '%s:%s' \n Message (len = %d): '%s'\n", peerIP, peerPort, (int) numbytes, msg);
		memcpy(addr_share, messaggio, sizeof(messaggio));

		printf("Message (len = %d): '%s'\n", (int) numbytes, msg);

		//unlock region	
		region_lock.l_type = F_UNLCK;	//SI SETTA A UNLOCK
		if (fcntl(fd_lock, F_SETLKW, &region_lock) == -1)
		{
			fprintf(stderr, "Failed to unlock region 1\n");
		}
		else
		{
			printf("Process %d unlocked file\n", getpid());
		}
	}	//wend

	//quando quit=1, esco dal ciclo While e dealloco le risorse
	close(sockfd);

	//ora puoi scrivere sia sul fd, che sulla memoria

        close(fd_lock);
	close(fd_share);
	shm_unlink("sharedmemory");
	return 0;
}

/**@} */
