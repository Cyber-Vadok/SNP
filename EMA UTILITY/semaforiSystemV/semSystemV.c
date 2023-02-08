#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main() {
    // Creazione del semaforo
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (semid < 0) {
        perror("semget");
        exit(1);
    }

    // Inizializzazione del semaforo a 1
    if (semctl(semid, 0, SETVAL, 1) < 0) {
        perror("semctl");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Codice per il figlio
        printf("Figlio: in attesa del semaforo...\n");
        struct sembuf sem_lock = {0, -1, 0};
        semop(semid, &sem_lock, 1);
        printf("Figlio: semaforo acquisito\n");

        // Sezione critica
        printf("Figlio: esecuzione della sezione critica...\n");
        sleep(2);
        printf("Figlio: sezione critica completata\n");

        // Rilascio del semaforo
        struct sembuf sem_unlock = {0, 1, 0};
        semop(semid, &sem_unlock, 1);
        printf("Figlio: semaforo rilasciato\n");
    } else {
        // Codice per il padre
        printf("Padre: in attesa del semaforo...\n");
        struct sembuf sem_lock = {0, -1, 0};
        semop(semid, &sem_lock, 1);
        printf("Padre: semaforo acquisito\n");

        // Sezione critica
        printf("Padre: esecuzione della sezione critica...\n");
        sleep(2);
        printf("Padre: sezione critica completata\n");

        // Rilascio del semaforo
        struct sembuf sem_unlock = {0, 1, 0};
        semop(semid, &sem_unlock, 1);
        printf("Padre: semaforo rilasciato\n");
    }

	sleep(5);

    // Rimozione del semaforo
    if (semctl(semid, 0, IPC_RMID) < 0) {
        perror("semctl");
        exit(1);
    }

    return 0;
}
