#include <stdio.h>
#include <dirent.h>
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

#define BUFFSIZE 256

pthread_t tid[2];
pthread_mutex_t lock;

struct parameters_for_thread {
    int a;
    int b;
};

int my_global_var = 100;

void add_one_min_two(void *params){
	
	//faccio il cast della struttura per prendermi i parametri
	struct parameters_for_thread *p = (struct parameters_for_thread *) params;

	int one = p->a;
	int two = p->b;
	
	while(1){
		pthread_mutex_lock(&lock); //prendo il mutex
        printf("add_one_min_two get mutex... \n");
		my_global_var = my_global_var + one - two;
		printf("Valore attuale variabile add_one_min_two: %d\n",my_global_var);
		printf("add_one_min_two release mutex... \n\n");
		pthread_mutex_unlock(&lock); //rilascio il mutex
		sleep(1);
	}	
}

void min_one(int one){
	while(1){
		pthread_mutex_lock(&lock); //prendo il mutex
        printf("min_one get mutex... \n");
		my_global_var = my_global_var - one;
		printf("Valore attuale variabile min_one: %d\n",my_global_var);
		printf("min_one release mutex... \n\n");
		pthread_mutex_unlock(&lock); //rilascio il mutex
		sleep(1);
	}
}



int main(int argc, char *argv[]){

	srand(time(NULL));
	

	pthread_mutex_init(&lock, NULL); //inizializzo il mutex

	int r = rand() % 5;
	//passo un solo parametro alla thread
	printf("min_one: %d\n",r);
	pthread_create(&(tid[0]), NULL, &min_one, r); //faccio partire la thread

	//per la seconda thread necessito di passare due parametri e quindi devo utilizzare una struttura
	int a = rand() % 15;
	int b = rand() % 5;
	struct parameters_for_thread p = {a,b};
	printf("add_one_min_two: %d,%d\n\n",a,b);
	//passo la struttura di parametri
	pthread_create(&(tid[1]), NULL, &add_one_min_two, &p); //faccio partire la thread




	pthread_join(tid[0], NULL); //aspetto che finisca la thread
	pthread_join(tid[1], NULL); //aspetto che finisca la thread
	
	pthread_mutex_destroy(&lock); //elimino il mutex

	return 0;
}