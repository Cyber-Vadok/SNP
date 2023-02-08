#include "Utility.h"

char *multicastIPString = MGROUP;   // First:  multicast IP address
char *service = MPORT;             // Second: multicast port (or service)
  
struct addrinfo hints, *res, *p; 
int sockfd;

void udp_server_multicast(){

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
    	return;
	}
	

  	
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
		return;
	}

}

void udp_method_multicast(){

	char multicastMsg[BUFSIZE] = "Messaggio di prova!";
	sendto(sockfd, multicastMsg, strlen(multicastMsg), 0,  p->ai_addr, p->ai_addrlen);
	printf("Inviato: %s\n",multicastMsg);
	
  	close(sockfd);
  	freeaddrinfo(res); 	
  	

}


int main(int argc, char *argv[]) { 
  	
  	                  
  	udp_server_multicast();
  	udp_method_multicast();
  	
return 0;
}


