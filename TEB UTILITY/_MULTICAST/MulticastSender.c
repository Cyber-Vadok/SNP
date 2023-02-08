#include "Utility.h"

 /*
int setTTL(int sockfd, int TTL, int family){
  	
  	if (family == AF_INET6) 
  	{ 
  		// v6-specific
    	// The v6 multicast TTL socket option requires that the value be
    	// passed in as an integer
		u_int multicastHop = TTL;
    	if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &multicastHop, sizeof(multicastHop)) < 0)
    	{
    		perror("setsockopt(IPV6_MULTICAST_HOPS) failed: ");
    		return FAILURE;
		}
      return SUCCESS;
  	} 
  	else if (family == AF_INET) 
  	{ 	
		// v4 specific
    	// The v4 multicast TTL socket option requires that the value be
    	// passed in an unsigned char
    	//casting esplicito
    	u_char multicastTTL = (u_char)TTL;
    	if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &multicastTTL, sizeof(multicastTTL)) < 0)
    	{
    		perror("setsockopt(IP_MULTICAST_TTL) failed: ");
    		return FAILURE;
		}
		return SUCCESS;
	} 

//qui in caso di errore
return FAILURE;
}
*/

int main(int argc, char *argv[]) { 

  	char *multicastIPString = MGROUP;   //Indirizzo multicast come stringa, lo prendi da Utility.h
  	char *service = MPORT;             //Porta, la prendi da Utility.h
  	
  	struct addrinfo hints, *res, *p;                   
  	memset(&hints, 0, sizeof(hints)); 
  	
  	hints.ai_family = AF_UNSPEC;            // v4 or v6 is OK
  	hints.ai_socktype = SOCK_DGRAM;         // Only datagram sockets
  	hints.ai_protocol = IPPROTO_UDP;        // Only UDP please
  	hints.ai_flags |= AI_NUMERICHOST;       // Don't try to resolve address
	hints.ai_flags |= AI_NUMERICSERV;		//Don't try to resolve port       
		
  	int ecode = getaddrinfo(multicastIPString, service, &hints, &res);
  	if (ecode != 0)
  	{
    	printf("getaddrinfo() failed: %s", gai_strerror(ecode));
    	return FAILURE;
	}
	
  	int sockfd = -1;
  	
  	for (p=res; p != NULL; p=p->ai_next)
  	{
  		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
  		if (sockfd < 0)
    	{
    		perror("socket() failed: ");
    		continue;
		}
		break;
	}
	if (p == NULL)
	{
		printf("Unable to open socket for multicast transmission\n");
		freeaddrinfo(res);
		return FAILURE;
	}
	
	/*
  	ecode = setTTL(sockfd, multicastTTL, p->ai_family);
  	if (ecode == FAILURE)
  	{
  		perror("setsocktopt error for TTL: ");
  	}
  	*/
  	

	//------------------------------------------------------------------------------------------------------------
	
		
	ssize_t numBytes = 0;
	numBytes = sendto(sockfd, argv[1], strlen(argv[1]), 0,  p->ai_addr, p->ai_addrlen);
	if (numBytes < 0)
	{
		perror("sendto() failed: ");	
	}
	else if (numBytes != strlen(argv[1]))
	{
		printf("sendto() sent unexpected number of bytes\n");
	} 

	
		
	/*
	numBytes = recvfrom(sockfd, response, BUFSIZE -1 ,0, NULL, NULL);
	if (numBytes != -1){
		response[numBytes] = '\0';
		printf("Received %d bytes message '%s'\n", numBytes, response );
	} else {
		if (errno == EWOULDBLOCK || errno == EAGAIN){
			printf("recvfrom timeout\n");
		}
	}
	*/
	 
  	close(sockfd);
  	freeaddrinfo(res); 	
  	
return 0;
}


