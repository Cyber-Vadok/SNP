#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

sem_t *semaphore;

void start_semaphore_posix(){
	semaphore = sem_open("/my_semaphore", O_CREAT, 0666, 1);
}

void stop_semaphore_posix(){
	sem_close(semaphore);
 	sem_unlink("/my_semaphore");
}

int main() {
   
	start_semaphore_posix();

	printf("P2 process trying to acquire semaphore\n");
    sem_wait(semaphore);
    printf("P2 process acquired semaphore\n");
    sleep(5);
	printf("P2 process releasing semaphore\n");
    sem_post(semaphore);

	stop_semaphore_posix();

    
    return 0;
}


