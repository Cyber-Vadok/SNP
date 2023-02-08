#include "Utility.h"

char multicastAddrString[INET6_ADDRSTRLEN] = MGROUP; //multicast address
char multicastPortString[INET6_ADDRSTRLEN] = MPORT; //UDP port
struct addrinfo hints, *res, *ptr;
int sockfd;

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


void udp_client_multicast(){

	printf("Multicast Receiver for <%s:%s>\n", multicastAddrString, multicastPortString); 

	int result = 0;

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
    	return;
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
		printf(" error for %s, %s\n", multicastAddrString, multicastPortString);
		freeaddrinfo(res);
		return;	
	}

	//sockfd is binded to the multicast address and port
	//now we must join the group
	
	result = join_group(sockfd, ptr->ai_addr, ptr->ai_addrlen);
	if (result == FAILURE)
	{
		//chiudo e dealloco
		close(sockfd);
		freeaddrinfo(res);
		return;
	}
	
}

void udp_method_multicast(){
	
	char ReceivedMessage[BUFSIZE] = " ";
	struct sockaddr_storage source;
	socklen_t source_len = sizeof(source);
	
	recvfrom(sockfd, ReceivedMessage, BUFSIZE-1, 0,(struct sockaddr *)&source, &source_len);
	printf("Ricevuto: %s\n",ReceivedMessage);
	
	leave_group(sockfd, ptr->ai_addr, ptr->ai_addrlen);
	
	//chiudo e dealloco
	close(sockfd);
	freeaddrinfo(res);

}



int main(int argc, char *argv[]){
	
	udp_client_multicast();
	udp_method_multicast();
	
return 0;
}



