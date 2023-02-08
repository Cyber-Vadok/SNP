/*
Costruire un programma C che prenda come argomento il percorso di una directory e che abbia due
thread:
1) il thread1 prende l'argomento e trova tutti i file regolari il cui percorso 
è radicato nell'argomento e ha un numero di collegamenti superiore a 1.
2) il thread2 prende ogni file trovato e cerca i file con lo stesso inode. 
del file trovato nella stessa gerarchia in cui è stata effettuata la ricerca.

I due thread sono sincronizzati correttamente.
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
#include <sys/stat.h>
#include <pthread.h>

pthread_t tid[2];
pthread_mutex_t lock;
char path[1024];
int count = 1;
int ino = 0;

void t1(){

	DIR *folder;
	struct stat *buf; 
    struct dirent *entry;
    char cwd[1024];
    char savepath[1024] = "";
    strcpy(savepath,path);
	
    folder = opendir(path);
    //getcwd(cwd, sizeof(cwd));
    //printf("\nStai cercando nella direcotory: %s\n",path);

    while(entry = readdir(folder))
    {
        if (entry->d_type == 8){
        	buf = malloc(sizeof(struct stat));
        	stat(path, buf);
        	if (buf->st_nlink > 1){
        		pthread_mutex_lock(&lock); //prendo il mutex
        		printf("T1 get mutex... \n");
        		printf("Regular %d: %s\n",count,entry->d_name);
        		strcat(path,"/");
				strcat(path,entry->d_name);
				printf("Path: %s\n",path);
				printf("Inode: %llu\n", entry->d_ino);
				printf("Record lenght: %d\n", entry->d_reclen);
				printf("Hard link: %d\n", buf->st_nlink);
				ino = (int)entry->d_ino;
				count++;
				pthread_mutex_unlock(&lock); //rilascio il mutex
				printf("T1 release mutex... \n\n");
				
        	}
			free(buf);
			strcpy(path,savepath);
        }
        
        if (entry->d_type == 4){
        	if (strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0){
        		//printf("Directory: %s\n",entry->d_name);
        		strcat(path,"/");
        		strcat(path,entry->d_name);
        		//printf("Direcotry: %s\n",path);
        		t1();
        		strcpy(path,savepath);
        	}
        }
        
        if (entry->d_type != 8 && entry->d_type != 4){
        	printf("File: %s\n",entry->d_name);
        }
	}
	closedir(folder);
}

int main(int argc, char *argv[]){

	strcpy(path,"/Users/emanuelemele/Desktop/Altro");
	
	pthread_mutex_init(&lock, NULL); //inizializzo il mutex
	pthread_create(&(tid[0]), NULL, &t1, NULL); //faccio partire la thread
	//pthread_create(&(tid[1]), NULL, &t2, NULL);
	
	pthread_join(tid[0], NULL); //aspetto che finisca la thread
	//pthread_join(tid[1], NULL);
	
	
	pthread_mutex_destroy(&lock);
	//t1(path);

	return 0;
}