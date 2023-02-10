/**
@addtogroup Group6
@{
*/
/**
@file 	Header.h
@author Catiuscia Melle

@brief 	Header comune al client e server dell'esempio

L'header definisce costanti comuni al client e server.
*/

#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h> //necessario su Mac
#include <stdlib.h>

#include <string.h>


#define PORT 49152 	/**< TCP listening port */
#define BACKLOG 10 	/**< dimensione della coda di connessioni */

#define BUFSIZE 256 /**< dimensione del buffer di messaggio */

#define FAILURE 3 	/**< definizione del valore di errore di ritorno del processo 
						in caso di errori delle Sockets API */

#define NOBYEMSG 	0/**< costante che inibisce l'invio di un messaggio di Bye dopo la ricezione di EoF */
#define SENDBYEMSG 	1 /**< costante che determina l'invio di un messaggio di Bye dopo la ricezione di EoF */

#endif /* __HEADER_H__ */

/** @} */