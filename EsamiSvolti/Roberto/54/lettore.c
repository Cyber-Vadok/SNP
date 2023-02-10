#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#define SIZE 400

int main(int argc, char
	const *argv[])
{

        off_t newpos = 1;
        off_t oldpos = 0;

	//MEMORIA CONDIVISA POSIX

	void *addr_share;
	int fd_share;

	//INIZIO BLOCCO REGION LOCK
	int fd_lock;
	struct flock region_lock;

	//NOTA SE HAI OCREAT E OEXCL E IL FILE ESISTE GIA, LA OPEN FALLISCE
	fd_lock = open("lock_file", O_RDWR | O_CREAT, 0666);
	if (!fd_lock)
	{
		fprintf(stderr, "Unable to open lockfile for read/write\n");
		exit(EXIT_FAILURE);
	}

              //settiamo il lock
	region_lock.l_whence = SEEK_SET;
	region_lock.l_start = 0;
	region_lock.l_len = 8;

	//creo la memoria condivisa, che ritorna il fd corrispondente
	fd_share = shm_open("/sharedmemory", O_RDWR, 0666);	//PER LEGGERE TOGLI O_CREAT
	if (fd_share == -1)
	{
		printf("Errore nel creare la memoria condivisa posix");
		return -1;
	}

	addr_share = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_share, 0);
	if (addr_share == MAP_FAILED)
	{
		printf("Errore nel mappare la memoria condivisa posix");
		return -1;
	}

	//ora puoi scrivere sia sul fd, che sulla memoria
	char readbuf[400];
	lseek(fd_share, 0, SEEK_SET);

	while (1)
	{
	        sleep(1);
		//lock region
		region_lock.l_type = F_WRLCK;	//si setta a lettura e scrittura
		if (fcntl(fd_lock, F_SETLKW, &region_lock) == -1)
		{
			fprintf(stderr, "Failed to lock region 1\n");
		}
                            int fd_file = open("file1.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
                            sleep(1);
		            printf("Process %d locked file\n", getpid());
		            //lseek(fd_share, 0, SEEK_SET);

		            printf("%s\n", addr_share);
		            dprintf(fd_file, "%s\n", addr_share);
		            
		            //unlock region	
		            
		            region_lock.l_type = F_UNLCK;	//SI SETTA A UNLOCK
		            if (fcntl(fd_lock, F_SETLKW, &region_lock) == -1)
		            {
			            fprintf(stderr, "Failed to unlock region 1\n");
		            }
		            else
		            {
			            printf("Process %d unlocked file\n", getpid());
		            }
	            
                            close(fd_file);
		}
	

	close(fd_lock);
	close(fd_share);
	shm_unlink("sharedmemory");

	//FINE BLOCCO MEMORIA CONDIVISA

	return 0;
}
