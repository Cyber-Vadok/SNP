#include "Utility.h"

//Per fare il join bisogna mettere una socket option IP_ADD_MEMBERSHIP che prende la struttura  di tipo ip_mreq riempita a dovere
int join_group(int sockfd, struct sockaddr *multicastAddr, socklen_t addrlen){

    struct ip_mreq joinRequest;
    joinRequest.imr_multiaddr = ((struct sockaddr_in *) multicastAddr)->sin_addr;
    joinRequest.imr_interface.s_addr = htonl(INADDR_ANY); //interfaccia, INADDR_ANY significa che è scelta automaticamente
    	    	
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &joinRequest, sizeof(joinRequest)) < 0)
    {
    	perror("setsockopt(IPV4_ADD_MEMBERSHIP) failed: ");
    	return -1;
	}
    //printf("Joining IPv4 multicast group...\n");
	return 0;
}
 
//Funzione per abbandonare un indirizzo multicast
int leave_group(int sockfd, struct sockaddr *multicastAddr, socklen_t addrlen){
	
		struct ip_mreq leaveRequest;
		leaveRequest.imr_multiaddr = ((struct sockaddr_in *) multicastAddr)->sin_addr;
    	leaveRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    	
    	return (setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &leaveRequest, sizeof(leaveRequest)));
}


int main(int argc, char *argv[]){
		
	printf("Multicast Receiver for <%s:%s>\n", MGROUP, MPORT); 
	
	int sockfd = -1;
	
	struct addrinfo hints, *res, *ptr;
	memset(&hints, 0, sizeof(hints)); 

	hints.ai_family = AF_UNSPEC;      // v4 or v6 is OK
  	hints.ai_socktype = SOCK_DGRAM;   // Only datagram sockets
  	hints.ai_protocol = IPPROTO_UDP;  // Only UDP protocol
  	hints.ai_flags |= AI_NUMERICHOST; // Don't try to resolve address
	hints.ai_flags |= AI_NUMERICSERV; // Don't try to resolve port

	int ecode = getaddrinfo(MGROUP, MPORT, &hints, &res);
  	if (ecode != 0)
  	{
    	printf("getaddrinfo() failed: %s", gai_strerror(ecode));
    	return FAILURE;
	}
	
	int result = 0;
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
	{	
		freeaddrinfo(res);
		return FAILURE;
	}

	if (join_group(sockfd, ptr->ai_addr, ptr->ai_addrlen) == FAILURE)
	{
		close(sockfd);
		freeaddrinfo(res);
		return FAILURE;
	}
	
	
	int quit = 0;
	ssize_t nbytes = 0;
	char receivedMessage[BUFSIZE] = "";

	struct sockaddr_storage source; //in questa struttura memorizziamo il mittente del messaggio
	socklen_t source_len = sizeof(source);

	/*
	int niflags = NI_NUMERICHOST | NI_NUMERICSERV;
	char ip[INET6_ADDRSTRLEN] = "";
	char port[INET6_ADDRSTRLEN] = "";
	*/
	
	
	
	while (!quit)
	{
		nbytes = recvfrom(sockfd, receivedMessage, BUFSIZE-1, 0, (struct sockaddr *)&source, &source_len);
		if (nbytes < 0)
		{
			perror("recvfrom() error: ");
			quit = 1;
		}
		else
		{
			receivedMessage[nbytes] = 0; //si aggiunge il null
			
			//SE SI VUOLE CONOSCERE IL MITTENTE DEL MESSAGGIO
			/*
			ecode = getnameinfo((struct sockaddr *)&source, source_len, ip, INET6_ADDRSTRLEN, port,INET6_ADDRSTRLEN, niflags);
			if (ecode)
			{
				printf("getnameinfo error: %s\n", gai_strerror(ecode));
			}
			*/

			//printf("%s:-> %s\n", ip, ReceivedMessage);
			
			printf("Ricevuto %s\n", receivedMessage);
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


