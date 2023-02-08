/*
Esercizio

Scrivere un client C e un server S che lavorano insieme come segue:

Quando C invia a S la stringa "JPEG", S inizia una ricerca ricorsiva, 
all'interno della home directory dell'utente che lo ha lanciato il programma, alla ricerca di file di tipo
JPEG (cioè i file che iniziano con il "numero magico" FFD8) e invia il loro percorso al client C.
il loro percorso al client C.

Quando C invia a S la stringa "PNG", S avvia una ricerca ricorsiva 
all'interno della home directory dell'utente che l'ha lanciato il programma, alla ricerca di file di tipo
PNG (cioè i file che iniziano con il "numero magico" 89504E470D0A1A0A)
e ne rimanda il percorso al client C.

Sia il client che il server devono svolgere le loro funzioni SENZA utilizzare la chiamata di sistema
la chiamata di sistema system().
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

int files = 1;

void search(char *path,int sockfd,struct socklen_t *len,struct sockaddr_in client,char *extension){

	DIR *folder;
    struct dirent *entry;
    char cwd[1024];
    char savepath[1024] = "";
    strcpy(savepath,path);
    
    char counter[BUFSIZE]; //trasformare files in string
	
    folder = opendir(path);
    //getcwd(cwd, sizeof(cwd));
    //printf("\nStai cercando nella direcotory: %s\n",path);

    while(entry = readdir(folder))
    {
        if (entry->d_type == 8){
        	//printf("Regular: %s\n",entry->d_name);
        	//search extension
        	char *ext = strrchr(entry->d_name, '.');
			if(ext != NULL){
				if(strcmp(ext+1,extension) == 0){
				printf("Regular %d: %s\n",files,entry->d_name);
				
				char tmpString[1024] = "Regular ";
				sprintf(counter, "%d", files);
				strcat(tmpString,counter);
				strcpy(counter,"");
				strcat(tmpString,": ");
				strcat(tmpString,entry->d_name);
				strcat(tmpString,"\n");
				//sendto(sockfd, tmpString, strlen(tmpString), 0, (struct sockaddr *)&client, len);
				strcmp(tmpString,"");
				
				strcat(path,"/");
				strcat(path,entry->d_name);
				printf("Path: %s\n\n",path);
				
				strcat(tmpString,"Path: ");
				strcat(tmpString,path);
				strcat(tmpString,"\n");
				sendto(sockfd, tmpString, strlen(tmpString), 0, (struct sockaddr *)&client, len);
				strcpy(tmpString,"");
				
				files++;
				strcpy(path,savepath);
    			//printf("extension is %s\n", ext + 1);
				}
			} 
			
        }
        if (entry->d_type == 4){
        	if (strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0){
        		//printf("Direcotry: %s\n",entry->d_name);
        		strcat(path,"/");
        		strcat(path,entry->d_name);
        		//printf("Direcotry: %s\n",path);
        		search(path,sockfd,len,client,extension);
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

	char buffer[BUFSIZE] = "";
	struct sockaddr_in client;
	char address[INET_ADDRSTRLEN] = "";
	char path[1024] = "/Users/emanuelemele/Desktop";
	
	int quit = 1;
	while (quit == 1){
		recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
		printf("Ricevuto messaggio:'%s' da: %s:%d\n", buffer, inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN), ntohs(client.sin_port) );
		printf("Ricerca dei file con l'estensione indicata ....\n");
		search(path,sockfd,len,client,buffer);
		
		sendto(sockfd,"stop",strlen("stop"), 0, (struct sockaddr *)&client, len); //messaggio finale di chiusura
		
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