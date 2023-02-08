#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define BUFFER 256

struct flock lock; /* struct for record lock*/
int fd; /* file descriptor for record lock */

void create_record_lock(){
  lock.l_type    = F_WRLCK; // F_RDLCK; per la lettura
  lock.l_start   = 0;
  lock.l_whence  = SEEK_SET;
  lock.l_len     = 0;  //50; per la lettura       
}

void record_lock(){
	lock.l_type = F_WRLCK; //prendo il record lock
	if(fcntl(fd, F_SETLKW, &lock) == -1){
		printf("Error to lock\n");
	}else{
		printf("Lock preso..\n");
	}
}

void record_unlock(){
	if(fcntl(fd, F_SETLK, &lock) == -1){
		printf("Error to unlock\n");
	}else{
		printf("Lock lasciato\n");
	}
}

void* create_shared_memory_posix(char* name){
	
	int SIZE = 4096;

	/* create the shared memory object */
	int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); 
	
	/* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE);
	
	/* memory map the shared memory object */
    void* ptr = mmap(0, SIZE, PROT_WRITE | PROT_READ , MAP_SHARED, shm_fd, 0);
	
	return ptr;
}

int main(int argc, char *argv[]){

	char buf[BUFFER] = "Ecco la stringa condivisa";

	//inizialize record lock
	create_record_lock();
	int fd = open("data.txt", O_RDONLY|O_APPEND); //or O_WRONLY|O_APPEND 

	/* inizialize shared memory*/
	void* shmem = create_shared_memory_posix("memCond");
	
	int counter = 0;
	while(1){
		record_lock();
		memcpy(buf,shmem,sizeof(buf));
		printf("%d: Il processo 2 ha letto: %s\n",counter,shmem);

		strcpy(buf,"Ecco la seconda stringa condivisa !");
		
		memcpy(shmem, buf, sizeof(buf));
		printf("%d: Il processo 2 ha scritto: %s\n",counter,shmem);
		strcpy(buf,"");
		record_unlock();
		counter++;
		printf("\n");
		
		sleep(1);
	}
	

	/* remove the shared memory object */
	shm_unlink("memCond");

	return 0;
}