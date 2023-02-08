#include "Utility.h"

//Funzione per fare la join ad un indirizzo multicast
int join_group(int sockfd, struct sockaddr *multicastAddr, socklen_t addrlen){

    	struct ip_mreq joinRequest;

    	//set the multicast IPv4 address
    	joinRequest.imr_multiaddr = ((struct sockaddr_in *) multicastAddr)->sin_addr;
    	
    	//the system choose the ingoing interface - wildcard IPv4 address
    	//quando si fa una bind di INADDR_ANY: sarà il sistema a scegliere l'interfaccia 
    	joinRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    	
    	printf("Joining IPv4 multicast group...\n");
    	
    	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &joinRequest, sizeof(joinRequest)) < 0)
    	{
    		perror("setsockopt(IPV4_ADD_MEMBERSHIP) failed: ");
    		return -1;
		}

		return 0;

}
 
//Funzione per abbandonare un indirizzo multicast
int leave_group(int sockfd, struct sockaddr *multicastAddr, socklen_t addrlen){
	
		// Leave the multicast v4 "group"
		struct ip_mreq leaveRequest;
		leaveRequest.imr_multiaddr = ((struct sockaddr_in *) multicastAddr)->sin_addr;
    	leaveRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    	
    	return (setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &leaveRequest, sizeof(leaveRequest)));
}

void move(char move);
void initializeMap();
void printMap();

char map[DIMENSION][DIMENSION];
int player_X = 5;
int player_Y = 5;

int main(int argc, char *argv[]){
	
	char multicastAddrString[INET6_ADDRSTRLEN] = MGROUP; //multicast address
	char multicastPortString[INET6_ADDRSTRLEN] = MPORT; //UDP port

	printf("Multicast Receiver for <%s:%s>\n", multicastAddrString, multicastPortString); 
	
	int result = 0;
	int sockfd = -1;
	
	struct addrinfo hints, *res, *ptr;

	//Zero out hints
	memset(&hints, 0, sizeof(hints)); 

  	//resolution hints:
	hints.ai_family = AF_UNSPEC;      // v4 or v6 is OK
  	hints.ai_socktype = SOCK_DGRAM;   // Only datagram sockets
  	hints.ai_protocol = IPPROTO_UDP;  // Only UDP protocol
  	hints.ai_flags |= AI_NUMERICHOST; // Don't try to resolve address
	hints.ai_flags |= AI_NUMERICSERV; // Don't try to resolve port

	int ecode = getaddrinfo(multicastAddrString, multicastPortString, &hints, &res);
  	if (ecode != 0)
  	{
    	printf("getaddrinfo() failed: %s", gai_strerror(ecode));
    	return FAILURE;
	}
	
	for (ptr = res; ptr != NULL; ptr=ptr->ai_next)
	{
		//open UDP socket
		sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sockfd == -1)
		{	
			perror("socket() error: ");
			continue;	
		}
		
		//opzione per il riuso dell'indirizzo
		int reuse_on = 1;
	
		//l'opzione SO_REUSEADDR permette di fare più bind allo stesso indirizzo
		result = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_on, sizeof(reuse_on));
		if (result == -1)
		{
			perror("setsockopt() error: ");
			close(sockfd);
			continue;
		}
			
		//bind socket to multicast address and port
		result = bind(sockfd, ptr->ai_addr, ptr->ai_addrlen);
		if (result < 0)
		{
			perror("bind() error: ");
			close(sockfd);
			continue;
		}
		break;
	}//for
	
	if (ptr == NULL)
	{	/* errno set from final socket() */
		printf("%s error for %s, %s\n", argv[0], multicastAddrString, multicastPortString);
		freeaddrinfo(res);
		return FAILURE;
	}

	//sockfd is binded to the multicast address and port
	//now we must join the group
	
	result = join_group(sockfd, ptr->ai_addr, ptr->ai_addrlen);
	if (result == FAILURE)
	{
		//chiudo e dealloco
		close(sockfd);
		freeaddrinfo(res);
		return FAILURE;
	}
	
	
	//handle_multicast_session(sockfd);
	int quit = 0;
	ssize_t nbytes = 0;
	char ReceivedMessage[BUFSIZE] = "";

	struct sockaddr_storage source;
	socklen_t source_len = sizeof(source);

	//printf("Ready for receiving multicast traffic\n");
	int niflags = NI_NUMERICHOST | NI_NUMERICSERV;
	char ip[INET6_ADDRSTRLEN] = "";
	char port[INET6_ADDRSTRLEN] = "";
	
	
	initializeMap();
	
	
	while (!quit)
	{
		nbytes = recvfrom(sockfd, ReceivedMessage, BUFSIZE-1, 0,\
						 (struct sockaddr *)&source, &source_len);
		if (nbytes < 0)
		{
			perror("recvfrom() error: ");
			quit = 1;
		}
		else
		{
			ReceivedMessage[nbytes] = 0;
			//getnameinfo si usa per conoscere ip e porta facilmente, ora le variabili ip e porta sono avvalorate
			ecode = getnameinfo((struct sockaddr *)&source, source_len, \
					ip, INET6_ADDRSTRLEN, port,INET6_ADDRSTRLEN, niflags);
			if (ecode)
			{
				printf("getnameinfo error: %s\n", gai_strerror(ecode));
			}
	
			if(strlen(ReceivedMessage) == 1){
				move(*ReceivedMessage);
				printMap();
			}else{
				printf("Mossa non valida\n");
			}

			
		}
	}//wend
		
	
	result = leave_group(sockfd, ptr->ai_addr, ptr->ai_addrlen);
	if (result == -1)
	{
		perror("setsockopt error: ");
	}
	
	
	//chiudo e dealloco
	close(sockfd);
	freeaddrinfo(res);
	
return 0;
}

void initializeMap(){
	printf("\n La mappa è formata da %lu blocchi, :\n\n", sizeof(map));
	for (int i = 0; i < DIMENSION; i++){
		for (int j = 0; j< DIMENSION; j++){
			if (i == (player_X-1) && j == (player_Y-1)){
					map[i][j] = 'X';
			}else{
					map[i][j] = '.';
					}
			printf("%c\t", map[i][j]);
			}
		printf("\n");
	}

}

void printMap(){
	for (int i = 0; i < DIMENSION; i++){
		for (int j = 0; j < DIMENSION; j++){
			printf("%c\t", map[i][j]);
			}
		printf("\n");
	}
}

void move(char move){
	printf("Ricevuto: %c\n", move);
	
	if (move == 'w'){
		map[player_X-1][player_Y-1] = '.';
		player_X -= 1; //NOTA: ANDARE SOPRA, SIGNIFICA CAMBIARE RIGA (andare sulla riga precedente)
		map[player_X-1][player_Y-1] = 'X';
	}else if (move == 'a'){
		map[player_X-1][player_Y-1] = '.';
		player_Y -= 1;
		map[player_X-1][player_Y-1] = 'X';
	}else if (move == 'd'){
		map[player_X-1][player_Y-1] = '.';
		player_Y += 1;
		map[player_X-1][player_Y-1] = 'X';
	}else if (move == 's'){
		map[player_X-1][player_Y-1] = '.';
		player_X += 1;
		map[player_X-1][player_Y-1] = 'X';
	}else{
		printf("Mossa non valida.\n");
	}

}

