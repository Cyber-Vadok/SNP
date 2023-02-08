#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>

#define PORTUDP 49152
#define BUFSIZE 256
#define BACKLOG 10

void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del client)
int udp_method(int sockfd,struct socklen_t *len, struct sockaddr_in server,char *argv[]){
	
	char msg[BUFSIZE] = "";
	void* shmem = create_shared_memory(128);
	
	sendto(sockfd, argv[1], strlen(argv[1]), 0, (struct sockaddr *)&server, len);
	strncpy(msg, "", sizeof(msg));
	recvfrom(sockfd, msg, BUFSIZE-1, 0, (struct sockaddr *)&server, &len);
	printf("Messaggio ricevuto da P1-T1: %s \n",msg);	
	memcpy(shmem, msg, sizeof(msg));
	
    int pid = fork();
  	if (pid == 0) { //child (P3)
    	printf("P3 invia la stringa a P1-T2\n");
    	sendto(sockfd, shmem, strlen(shmem), 0, (struct sockaddr *)&server, len);
  	} else { //parent (P2)
    	//printf("Parent read: %s\n", shmem);
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
	udp_method(sockfd,len,server,argv);
	
	exit(0);
}


int main(int argc, char *argv[]){
	
	udp_client(argc,argv,PORTUDP);
	
	return 0;
}