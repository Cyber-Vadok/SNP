#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTTCP 49151 	/**< TCP listening port */
#define PORTUDP 49152	/**< UDP listening port */
#define BACKLOG 10 	/**< dimensione della coda di connessioni */

#define BUFSIZE 256 /**< dimensione del buffer di messaggio */

#define FAILURE 3 	/**< definizione del valore di errore di ritorno del processo 
						in caso di errori delle Sockets API */



#endif 
