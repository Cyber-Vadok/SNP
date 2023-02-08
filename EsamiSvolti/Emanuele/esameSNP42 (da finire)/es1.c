/*
Scrivere un programma che verifichi quali elementi della struttura stat 
siano supportati per un file di tipo socket. 
*/

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PORTUDP 49152
#define BUFSIZE 256
#define BACKLOG 10

//Creazione del server UDP
int udp_server(int port){
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in server;
	socklen_t len = sizeof(server);
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address Family IPv4
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	bind(sockfd, (struct sockaddr *)&server, len);
	printf("\nServer UDP listening on port %d\n", (int)port);
	
	return sockfd;
}


//Le strutture per ricavare informazioni sui file sono stat e dirent
int main(int argc, char *argv[]){

	int sockfd = udp_server(PORTUDP);
	
	struct stat *buf;
	
	buf = malloc(sizeof(struct stat));
	fstat(sockfd, buf);
	
	printf("Questi sono gli elementi di una struttura stat per un socket: \n");
	
	printf("Serial number for the file: %llu\n",buf->st_ino);
	printf("Access mode and file type for the file: %hu\n",buf->st_mode);
	printf("Number of links to the file: %hu\n",buf->st_nlink);
	printf("User ID of file owner: %u\n",buf->st_uid);
	printf("Device ID : %d\n",buf->st_rdev);
	printf("Group ID of group owner: %u\n",buf->st_gid);
	printf("File size in bytes: %lld\n",buf->st_size);
	printf("Time of last access:  %ld\n",buf->st_atime);
	printf("Time of last data modification: %ld\n",buf->st_mtime);
	printf("Time of last file status change: %ld\n",buf->st_ctime);
	printf("A file system-specific preferred I/O block size for this object: %d\n",buf->st_blksize);
	printf("Number of blocks allocated for this file: %lld\n",buf->st_blocks);
	
	
	close(sockfd);
	free(buf);
	
	return 0;
}