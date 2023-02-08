/*
Esercizio

Creare un daemon che periodicamente effettua un mirroring dell'intero
contenuto di una directory in un altra directory (passate come argomento
sulla linea di comando) copiando solo i file che sono stati modificati
o cambiati all'ultimo backup e supponendo che i file possano essere solo
cambiati o modificati ma mai rimossi o aggiunti. 
Confrontare le performance del programma nel caso in cui le operazioni 
di verifica e copia siano affidate a delle thread oppure no.

*/

#include <stdio.h>
#include <dirent.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

pthread_t tid[2];
pthread_mutex_t lock;

#define BUFFSIZE 256

DIR *folder;
struct dirent *entry;
struct stat *buf; 
char *destination_path[1024];

void copyfile(char *path_file,char *path_destination){

	// Apre il file originale
	FILE *file1 = fopen(path_file, "r");
	// Apre il file di destinazione
	FILE *file2 = fopen(path_destination, "w+");	

	// Controlla se i file sono stati aperti correttamente
	if (file1 == NULL || file2 == NULL) {
 		printf("Errore nell'apertura dei file!\n");
    	exit(1);
	}

	// Buffer per i dati del file
	char buffer[2048];

	// Legge i dati dal file originale e li scrive nel file di destinazione
	while (fgets(buffer, sizeof(buffer), file1) != NULL) {
    	fputs(buffer, file2);
	}

	// Chiudi i file
	fclose(file1);
	fclose(file2);
}

void check_last_update_file(){

	char checkpath[1024] = "./";
	folder = opendir(checkpath);

	int counter_file = 0;

	//controllo quanti file ci sono nella directory da controllare
	while(entry = readdir(folder)){

		//cerco solo file regolari
		if (entry->d_type == 8){
			counter_file++;
		}

	}

	//creo sufficienti stringhe per memorizzare il tempo di ultima modifica di ogni file
	char last_mod_string[counter_file][BUFFSIZE];
	//resetto il contatore
	counter_file = 0;

	//apro nuovamente la directory per poterla rileggere
	folder = opendir(checkpath);

	while(1){

		//apro nuovamente la directory per poterla rileggere
		folder = opendir(checkpath);

		while(entry = readdir(folder)){

		//cerco solo file regolari
			if (entry->d_type == 8){

				buf = malloc(sizeof(struct stat));
				stat(strcat(checkpath,entry->d_name), buf);

				char buff[100];

				//formatto la data e l'orario
				strftime(buff, sizeof buff, "%D %T", gmtime(&buf->st_mtimespec.tv_sec));
				if(strcmp(last_mod_string[counter_file],buff) != 0){
					printf("File %d: %s\n",counter_file+1,entry->d_name);
					strcpy(last_mod_string[counter_file],buff);

					//creo la path dove mettere la copia
					char mycopy[256] = "";
					strcat(mycopy,destination_path);
					strcat(mycopy,"/");
					strcat(mycopy,entry->d_name);
					//copio
					pthread_mutex_lock(&lock); //prendo il mutex
					printf("T1 get mutex... \n");
					copyfile(checkpath,mycopy);
					pthread_mutex_unlock(&lock); //rilascio il mutex
					printf("T1 release mutex... \n");
					strcpy(mycopy,"");

					printf("Update last modification! \n");
					printf("Last modification: %s\n\n",buff);
				}
				
				/*
				strftime(buff, sizeof buff, "%D %T", gmtime(&buf->st_atimespec.tv_sec));
				printf("Last access: %s\n",buff);

				strftime(buff, sizeof buff, "%D %T", gmtime(&buf->st_ctimespec.tv_sec));
				printf("Last status change: %s\n\n",buff);
				*/

				strcpy(checkpath,"./");
				counter_file++;
			}
		}

		printf("\n");

		/*
		for (int i = 0; i < counter_file; i++)
		{
			printf("File %d: %s\n",i,last_mod_string[i]);
		}
		*/

		counter_file = 0;
		sleep(1);
	}

}

int main(int argc, char *argv[]){

	// change to the "/" directory
	int nochdir = 0;

	// using my current working directory
	nochdir = 1;

	// redirect standard input, output and error to /dev/null
	// this is equivalent to "closing the file descriptors"
	int noclose = 0;

	// start deamon
	daemon(nochdir, noclose);
	
	//scrivo nei log
	//devo avere lo stesso nome dell'eseguibile 
	//per vedere i log scritti devo usare il comando log show --predicate 'process == "deamon"'
	openlog("deamon", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_NOTICE, "Start deamon!!!");
	
    closelog();
	
	
	

	//argv[1] è la path di destinazione dove si vogliono copiare i file, e va espressa come /path_dir
	strcpy(destination_path,argv[1]);

	pthread_mutex_init(&lock, NULL); //inizializzo il mutex

	pthread_create(&(tid[0]), NULL, &check_last_update_file, NULL); //faccio partire la thread

	pthread_join(tid[0], NULL); //aspetto che finisca la thread
	
	pthread_mutex_destroy(&lock); //elimino il mutex

	return 0;
}