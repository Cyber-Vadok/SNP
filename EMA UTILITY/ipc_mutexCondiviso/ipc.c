    #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/my_shared_memory"

//struttura che contiene il mutex condiviso dai due processi e il fato che vogliamo elaborare in comune
typedef struct {
    pthread_mutex_t mutex;
    int shared_variable;
} shared_data_t;

//dati e strutture per la memoria condivisa
int shm_fd;
shared_data_t *shared_data;

void create_shared_memory_mutex(){

	/* Crea o apre una memoria condivisa */
	shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1) {
		perror("shm_open");
		exit(1);
	}

	/* Alloca la memoria condivisa */
    if (ftruncate(shm_fd, sizeof(shared_data_t)) == -1) {
        perror("ftruncate");
        exit(1);
    }

	/* Mappa la memoria condivisa nello spazio di indirizzamento del processo */
    shared_data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

	/* Inizializza il mutex nella memoria condivisa */
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared_data->mutex, &attr);

}

void remove_shared_memory_mutex(){
	/* rimuove la memoria condivisa */
	if(shm_unlink(SHM_NAME) == -1) {
		perror("shm_unlink");
		exit(1);
	}
}

int main() {
    
    create_shared_memory_mutex();

	/* Crea due processi figli */
    pid_t pid1 = fork();
    if (pid1 == 0) {
        /* Il primo processo figlio acquisisce il mutex e incrementa la variabile condivisa */
        pthread_mutex_lock(&shared_data->mutex);
        shared_data->shared_variable++;
        printf("Child process 1: shared variable is now %d\n", shared_data->shared_variable);
        pthread_mutex_unlock(&shared_data->mutex);
        exit(0);
    } else {
        pid_t pid2 = fork();
        if (pid2 == 0) {
            /* Il secondo processo figlio acquisisce il mutex e incrementa la variabile condivisa */
            pthread_mutex_lock(&shared_data->mutex);
            shared_data->shared_variable++;
            printf("Child process 2: shared variable is now %d\n", shared_data->shared_variable);
            pthread_mutex_unlock(&shared_data->mutex);
            exit(0);
        } else {
            /* Il processo padre aspetta che i figli terminino */
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            printf("Parent process: shared variable is now %d\n", shared_data->shared_variable);
			remove_shared_memory_mutex();
        }
    }
    return 0;
}
