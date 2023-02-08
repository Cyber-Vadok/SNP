#include "Utility.h"


int main(int argc, char *argv[]) { 

  	char *multicastIPString = MGROUP;   // First:  multicast IP address
  	char *service = MPORT;             // Second: multicast port (or service)
  	
  	struct addrinfo hints, *res, *p;                   
  	
	// Zero out structure
	memset(&hints, 0, sizeof(hints)); 
  	
	//resolution criteria
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
  		// Create socket 
  		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
  		if (sockfd < 0)
    	{
    		perror("socket() failed: ");
    		continue;
		}
		break;
	}//for
	
	if (p == NULL)
	{
		printf("Unable to open socket for multicast transmission\n");
		freeaddrinfo(res);
		return FAILURE;
	}
	
  	
	//invio multicast
	ssize_t numBytes = 0;
 	int quit = 0;

 	char multicastMsg[BUFSIZE] = "";
	
		
		// Multicast the string to all who have joined the group
		numBytes = sendto(sockfd, argv[1], strlen(argv[1]), 0,  p->ai_addr, p->ai_addrlen);
		if (numBytes < 0)
		{
			perror("sendto() failed: ");	
			quit = 1;
		}
		else if (numBytes != strlen(argv[1]))
		{
			printf("sendto() sent unexpected number of bytes\n");
			quit = 1;
		} 

	
 	//on close:
  	close(sockfd);
  	freeaddrinfo(res); 	
  	
return 0;
}

