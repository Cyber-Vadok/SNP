/*
Esercizio

Scrivere nel linguaggio C un programma client di nome A e un programma 
server di nome B che funzionino nel modo seguente:

1) Il programma A chiede all'utente se vuole contare file o directory.

2) L'utente risponde con una lettera ("f" per file e "d" per directory).

3) Il programma A invia con un datagramma UDP tale lettera al programma
B.

4) Il programma B conta tutti i file o tutte le directory della macchina
su cui gira, a seconda della lettera ricevuta e invia la risposta al
programma A che la mostra all'utente.
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

#define PORTUDP 49152
#define BUFSIZE 256
#define BACKLOG 10

int files = 0;
int dirs = 0;

void search_file(char *path,int sockfd,struct socklen_t *len,struct sockaddr_in client){

	DIR *folder;
    struct dirent *entry;
    char cwd[1024];
    char savepath[1024] = "";
    strcpy(savepath,path);
    
    char counter[BUFSIZE]; //trasformare files in string
	
    folder = opendir(path);

    while(entry = readdir(folder))
    {
        if (entry->d_type == 8){
		
			printf("Regular %d: %s\n",files,entry->d_name);
			
			char tmpString[1024] = "Regular ";
			sprintf(counter, "%d", files);
			strcat(tmpString,counter);
			strcpy(counter,"");
			strcat(tmpString,": ");
			strcat(tmpString,entry->d_name);
			strcat(tmpString,"\n");
			strcmp(tmpString,"");
			
			strcat(path,"/");
			strcat(path,entry->d_name);
			printf("Path: %s\n\n",path);
			
			strcat(tmpString,"Path: ");
			strcat(tmpString,path);
			strcat(tmpString,"\n");
			strcpy(tmpString,"");
			
			files++;
			strcpy(path,savepath);
        }
        if (entry->d_type == 4){
        	if (strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0){
				dirs++;
        		printf("Direcotry %d: %s\n",dirs,entry->d_name);
        		strcat(path,"/");
        		strcat(path,entry->d_name);
        		//printf("Direcotry: %s\n",path);
        		search_file(path,sockfd,len,client);
        		strcpy(path,savepath);
        	}
        }
        if (entry->d_type != 8 && entry->d_type != 4){
        	printf("File: %s\n",entry->d_name);
        }
	}
	closedir(folder);
	
}

//Metodo per far svolgere azioni al server (modificare solo questo per cambiare il comportamento del server)
int udp_method(int sockfd,struct socklen_t *len){

	char buffer[BUFSIZE] = " ";
	struct sockaddr_in client;
	char address[INET_ADDRSTRLEN] = " ";
	char path[1024] = "/Users/emanuelemele";

	recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
	printf("Client registrato %s:%d\n", inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN), ntohs(client.sin_port));
	//printf("Ricevuto messaggio:'%s' da: %s:%d\n", buffer, inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN), ntohs(client.sin_port) );

	strcpy(buffer," ");
	recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
	printf("Ho ricevuto: %s\n",buffer);

	search_file(path,sockfd,len,client);

	if (strcmp(buffer,"f") == 0){
		strcpy(buffer," ");
		sprintf(buffer, "%d", files);
		printf("\nTOTALE FILE: %s \n",buffer);
		sendto(sockfd,buffer,strlen(buffer), 0, (struct sockaddr *)&client, len); 
	}
	if (strcmp(buffer,"d") == 0){
		strcpy(buffer," ");
		sprintf(buffer, "%d", dirs);
		printf("\nTOTALE DIR: %s \n",buffer);
		sendto(sockfd,buffer,strlen(buffer), 0, (struct sockaddr *)&client, len); 
	}

	exit(0);
}

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
	printf("\n\tServer UDP listening on port %d\n\n", (int)port);
	
	//Richiamo del metodo scritto da noi
	udp_method(sockfd,len);
	
	exit(0);
}


int main(int argc, char *argv[]){

	int sockfd = udp_server(PORTUDP);
    return(0);
}