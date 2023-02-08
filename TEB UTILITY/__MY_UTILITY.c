#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>         
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/times.h>
#include <time.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <termios.h>
#include <semaphore.h>
 
int main(int argc, char *argv[]) {


    return 0;
}


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- 

	//THREADS

	//mutex senza attributi
	static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

	//creo adesso una condition variable con il relativo mutex
	//con questo mutex viene protetta una variabile, e con la condition variable si notifica il cambiamento a chi lo stava aspettando 
	static pthread_mutex_t mtx_cond = PTHREAD_MUTEX_INITIALIZER;
	static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


	static void * threadFunc(void *arg)
	{
		if (pthread_mutex_lock(&mtx) != 0)          
			printf("Errore nel prendere il mutex");
		
		if (pthread_mutex_unlock(&mtx) != 0) 
			printf("Errore nel rilasciare il mutex");
			
			
		/*
		Quando si vuole essere notificati di qualcosa usando la condition variable:
		chi vuole essere notificato prima prende il lock corrispondente, poi si mette ad aspettare (cosa che automaticamente sblocca il lock)
		successivamente qualcuno prenderà il lock, lo lascerà, e potrà notificare del cambiamento della variabile 
		in maniera atomica la thread che stava aspettando la notifica, si sveglia e prende il lock(dovrà infatti poi lasciarlo)
		*/		
		
		//thread che vuole essere notificato 	
         if (pthread_mutex_lock(&mtx_cond) != 0) //prendi lock
				printf("Errore nel prendere mutex per condition variable");
       
        if (pthread_cond_wait(&cond, &mtx_cond) != 0) //lascia lock e aspetta. quando qualcuno ti notifica riprendi il lock automaticamente.
				printf("Errore nell'aspettare la condition variable");
		
        if (pthread_mutex_unlock(&mtx_cond) != 0)//devi quindi poi rilasciare il lock
				printf("Errore nel rilasciare il mutex per la condition variable");	
				
		//thread che notifica
		//semplicemente prenderà e lascerà il mutex per fare cose 
		if (pthread_mutex_lock(&mtx_cond) != 0)
			printf("Errore nel prendere il mutex per aggiornare i risultati (occurrences)");
       	//fa cose
       	if (pthread_mutex_unlock(&mtx_cond) != 0) //rilascio il lock perchè è stata cambiata
			printf("Errore nel rilasciare il mutex per aggiornare i risultati (occurrences)");

		//notifica il thread in attesa
        if (pthread_cond_signal(&cond) != 0) //avviso chi voleva saperlo che la variabile è cambiata tramite la condition variable
			printf("Errore nella chiamata della condition variable");
			
			
			
		pthread_exit(NULL); //per uscire con valore di uscita che si prende dalla join	
	}  
	
	//variabile globale, ma che ha un valore diverso per ogni thread
	__thread int val;
	
	
	pthread_t t1;  
	
	if (pthread_create(&t1, NULL, threadFunc, NULL) != 0)
        printf("Errore nella creazione della thread");    
	
    if (pthread_join(t1, NULL) != 0)
        printf("Errore nella join del thread");    

    if (pthread_cancel(t1) != 0)
		printf("Errore nel cancellare la thread t3");
		
		
		
	/*
	//MUTEX AND CONDITION VARIABLE SHARED BETWEEN PARENT AND CHILD
	
	pthread_mutex_t* mutp;
	pthread_cond_t* cond_var;

	void* mmap_ptr  =  mmap (NULL, sizeof (pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	if (mmap_ptr == MAP_FAILED) {
  		perror ("mmap failed");
  		return -1;
	}

	mutp = (pthread_mutex_t*)mmap_ptr; //PERCHE COSI VA E NELL'ALTRO MODO NO

	// initialize the attribute
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_setpshared (&attr, PTHREAD_PROCESS_SHARED); // this is what we're testing

	// initialize the mutex
	pthread_mutex_init (mutp, &attr);
	pthread_mutexattr_destroy (&attr);





	void* mmap_ptr2  =  mmap (NULL, sizeof (pthread_cond_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	if (mmap_ptr2 == MAP_FAILED) {
  		perror ("mmap failed");
  		return -1;
	}

	cond_var = (pthread_cond_t*)mmap_ptr2; //PERCHE COSI VA E NELL'ALTRO MODO NO

	// initialize the attribute
    pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
	pthread_condattr_setpshared (&cond_attr, PTHREAD_PROCESS_SHARED); // this is what we're testing

	// initialize the mutex
	pthread_cond_init (cond_var, &cond_attr);
	pthread_condattr_destroy (&cond_attr);
	
	
	
	*/
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

	//COMMUNICATE WITH SIGNAL 
	
	/*
	Se due processi, parent e child per esempio, vogliono comunicare con dei segnali,
	è fondamentale che prima il parent blocchi il segnale interessato.
	In questo modo evitiamo race condition, in caso il segnale arrivi prima che il processo che lo vuole ricevere si metta in attesa.
	Questo funziona perchè le maschere di segnali vengono bloccati. 
	*/
	
	void sigusr_handler(int sig) {
    	//printf("Received sigusr signal\n");
	}

	//nel parent, o comunque alla nascita del processo
	sigset_t set;
    sigemptyset(&set); // svuoto il set    
    sigaddset(&set, SIGUSR1); // aggiungo SIGUSR1 
    sigprocmask(SIG_BLOCK, &set, NULL); // blocca il set di segnali 
    
    //nel processo che aspetta il segnale 
    //si crea poi il set che blocca tutti i segnali tranne quello che vogliamo che svegli il processo
    // questa maschera verrà temporaneamente importata al processo tramite la sigsuspend: il processo aspetta un segnale tra quelli non bloccati per svegliarsi
    sigset_t wake_me_up_set;
    sigfillset(&wake_me_up_set);  
	sigdelset(&wake_me_up_set, SIGUSR1);
	
	//NOTA CHE QUANDO ARRIVA IL SEGNALE CHE SVEGLIA LA SIGSUSPEND, SE NON E' GESTITO CI SARANNO CONSEGUENZE
    signal(SIGUSR1, sigusr_handler);    
    
    
	sigsuspend(&wake_me_up_set);

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- 

//INTER PROCESS COMMUNICATION

//----------------------------------------------------------------------------------------------- 


// FILE REGOLARE CARICATO IN MEMORIA

	void	*addr_share;
	int 	fd_share;
	
	//creiamo il file regolare NOTA: PER LETTURA TOGLIERE O_TRUNC (e eventualmente O_CREAT)
	if ((fd_share = open(FILE_NAME1, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0){
		printf("Errore nel creare il file share1.txt, fd: %d", fd_share);
		exit(-1);
		}
	
	//Si stabiliscono le dimensioni del file
	if (ftruncate(fd_share1, SIZE) != 0){
    	perror("ftruncate() error");
    	exit(-1);
    }
	
	//Si mappa il file in memoria.
	//NOTA MMAP LAVORA IN PAGINE, SE METTI 1 BYTE RIMANE COMUNQUE UNA PAGINA
	if ((addr_share = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_share1, 0)) == MAP_FAILED){
		printf("Errore nel mapping. \n");
		exit(-1);
	}
	
	
	//ora puoi scrivere sia sul fd, che sulla memoria
	close(fd_share);

	/*
	Da notare che con questo tipo di memoria condivisa, è necessario il truncate per stabilire le dimensioni del file per poter scrivere. 
	Se il file è vuoto non si può scrivere nulla tramite l'indirizzo di memoria a cui è mappato: scrivendo in memoria non si "allunga" la dimensione del file.
	Cio se il file è vuoto, tu scrivi in memoria ma non si riflette sul file. 
	Da notare che in questo caso la truncate su mac funziona.
	*/
	
//----------------------------------------------------------------------------------------------- 

// POSIX SHARED MEMORY 


	//PUOI CREARE UN NOME DIVEERSO OGNI RUN COSI EVITI IL PROBLEMA TRUNCATE E IL NOME CAMBIA SOLO. 
	//IN CASO DI FORK VA BENE MA SE SONO DUE PROCESSI DIVERSI OVVIAMENTE QUESTO NOME POI LO DEVI CONOSCERE
	
	/*
	char memname[64];
	time_t my_time = time(NULL);
	sprintf(memname, "%lu", my_time);
	*/
	
	void	*addr_share;
	int		fd_share;
	
	
	//creo la memoria condivisa, che ritorna il fd corrispondente
	fd_share = shm_open("/sharedmemory", O_RDWR | O_CREAT, 0666); //PER LEGGERE TOGLI O_CREAT
	if (fd_share == -1){
		printf("Errore nel creare la memoria condivisa posix");
		return -1;
		}

	
	//NOTA: QUESTO FTRUNCATE E' NECESSARIO, MA SU MAC LO PUOI FARE SOLO UNA VOLTA... quindi potresti dover cambiare chiave ogni run
	//NOTA LO FAI SOLO SU CHI APRE LA MEMORIA
	if (ftruncate(fd_share, SIZE) != 0){
    	perror("ftruncate() error");
    	printf("DEVI CAMBIARE IDENTIFICATIVO DELLA MEMORIA OPPURE TOGLIERE QUESTA TRUNCATE, CHE PUOI FARE ALLA MEMORIA SOLO UNA VOTLA");
    }
		
	addr_share = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_share, 0);
    if (addr_share == MAP_FAILED){
 		printf("Errore nel mappare la memoria condivisa posix");
		return -1;
	}
	
	//ora puoi scrivere sia sul fd, che sulla memoria

	close(fd_share);
	shm_unlink("sharedmemory");


//----------------------------------------------------------------------------------------------- 

// -> NON <- POSIX SHARED MEMORY

	//puoi creare la chiave basandosi sul nome di un file esistente, oppure mettere semplicemente un id numerico nella get
	//key_t mykey;
	//mykey2 = ftok( FILE_NAME2, 1);
	
	//si crea la memoria e si ottiene l'id
	int mem_id = shmget(1234, SIZE, IPC_EXCL | IPC_CREAT | 0666 ); 
	//NOTA IN LETTURA SI METTE SIZE = 0 E I FLAG ANCHE UGUALI A ZERO shmget( SH_NP, 0,  0 )
	
	void* addr_share = shmat( mem_id, NULL, 0 ); //si mappa la memoria


	--> //IMPORTANTISSIMO OGNI VOLTA DEVI RIMUOVERE LA MEMORIA OPPURE CAMBIARE L'ID ALTRIMENTI SEGMENTATION FAULT!!!!!
	
	
	shmdt( addr_share );
	
	//DOPO CHE HAI FINITO DI UTILIZZARE LA MEMORIA, PUOI RIMUOVERLA come segue
	shmctl( mem_id, IPC_RMID, addr_share3 ); 
 	/*
 	Da notare che in questo caso non ci sono file descriptor, e si decide la dimensione quando si crea la memoria. 
 	https://profs.scienze.univr.it/~cicalese/LSO/LSO08/Lez28May_shmem_sem.pdf
 	*/
 	
//----------------------------------------------------------------------------------------------- 

// CODE DI MESSAGGI


	int msgid = msgget((key_t)42969, 0666 | IPC_CREAT);

    if (msgid == -1) {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(-1);
    }

	//struttura che contiene messaggio e livello di importanza del messaggio
    struct my_msg_st {
    	long int my_msg_type;
    	char some_text[32];
	};

    struct my_msg_st some_data;    
	some_data.my_msg_type = 1;
	
	//NOTA: per riempire il campo della stringa potresti dover fare una strpcy
    strcpy(some_data.some_text, buffer); 


	//PER INVIARE
	if (msgsnd(msgid, (void *)&some_data, 32, 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(-1);
	} 
	 
	 
	 //PER RICEVERE
	 long int msg_to_receive = 0;
	 if (msgrcv(msgid, (void *)&some_data, 32, msg_to_receive, 0) == -1) {
            fprintf(stderr, "msgrcv failed with error: %d\n", errno);
            exit(-1);
	} //è cosi riempita la struttura some_data
	
	//PER RIMUOVERE LA CODA DI MESSAGGI
	if (msgctl(msgid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(-1);
    }

	/*
	 Da notare, che non è necessario sincronizzare i processi in questo caso. 
	 La msgrcv è bloccante, quindi se si arriva prima il ricevitore semplicemente aspetterà.
	 Se invece arriva la msgsend prima che il secondo processo stia ricevendo, il messaggio sarà comunque nella coda, e verrà tranquillamente letto quando si farà la send. 
	 */

//----------------------------------------------------------------------------------------------- 
    
    
	//SEMAFORI -> NON POSIX <-
    
	static int semaphore_p(int num_sem);
	static int semaphore_v(int num_sem);
	static int set_semvalue(void);
	
	
	//puoi creare la chiave di un semaforo creando una chiave con ftok passando il percorso di un file esistente
	//key_t my_key;
	//my_key = ftok(SEMAPHORE_KEY, 1); //SEMAPHOREKEY è il path di un file che deve esistere, e poi un identificativo
	
	//creazione del semaforo
	int sem_id = semget((key_t)6969420, 3, 0666 | IPC_CREAT); //si passa la chiave, IL NUMERO DI SEMAFORI (3), e i permessi (e flag)
	//NOTA SE SAI CHE ESISTE GIA IL SEMAFORO CON QUESTA CHIAVE PUOI METTERE 0 AL NUMERO DEI SEMAFORI NELLA SEMGET
	
	//INIZIALIZAZZIONE DEL SEMAFORO DA FARE SOLO IN UN PROCESSO 
	if (set_semvalue() != 0){
		return -1;
	}
	
	
	//PER PRENDERE IL SEMAFORO SI CHIAMA LA semaphore_p PASSANDO IL NUMERO DEL SEMAFORO DA PRENDERE
	//PER RILASCIARE IL SEMAFORO SI CHIAMA LA semaphore_v PASSANDO IL NUMERO DEL SEMAFORO DA RILASCIARE
	
	
	int set_semvalue()
	{
		/*
		Visto che ci sono più semafori si usa l'operazione SETALL in semctl, di conseguenza non deve cambiare il valore val di sem_union ma bisogna creare un array con un valore per ogni semaforo
		Da notare che il secondo valore di semctl è ignorato in molte situazioni, controlla quando lo usi se ti serve
		*/
	
		unsigned short vals[3];
		vals[0] = 1;
		vals[1] = 1;
		vals[2] = 1;
	
    	union semun sem_union;
    	sem_union.array = vals;
    
    	if (semctl(sem_id, 0, SETALL, sem_union) == -1){
    		printf("Errore nell'inizializazzione del semaforo.");
    		return(-1);
    	}
    	
    	return(0);
	}

	int semaphore_p(int num_sem)
	{
    	struct sembuf sem_b;
    
    	sem_b.sem_num = num_sem;
    	sem_b.sem_op = -1; /* P() */
    	sem_b.sem_flg = SEM_UNDO;
    	if (semop(sem_id, &sem_b, 1) == -1) {
        	fprintf(stderr, "Tentativo di prendere il semaforo fallito \n");
        return -1;
    	}
    	return 0;
	}


	int semaphore_v(int num_sem)
	{
    	struct sembuf sem_b;
    
    	sem_b.sem_num = num_sem;
    	sem_b.sem_op = 1; /* V() */
    	sem_b.sem_flg = SEM_UNDO;
    	if (semop(sem_id, &sem_b, 1) == -1) {
        	fprintf(stderr, "Tentativo di rilasciare il semaforo fallito\n");
			return -1;
    	}
		return 0;
	}

//----------------------------------------------------------------------------------------------- 
    
	//SEMAFORI POSIX-
	
	//creazione del semaforo 
	sem_t *sem;
    sem = sem_open("/my_semaphore", O_CREAT, 0666, 1); // 1 = maximum number of processes that can own the resource at the same time
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

	sem_wait(sem); //PRENDI IL SEMAFORO
   
	sem_post(sem);	//RILASCIA IL SEMAFORO

    sem_close(sem);	// CHIUDI IL SEMAFORO
	
	sem_unlink(sem) //DOVREBBE ELIMINARE IL SEMAFORO DAL SISTEM

//----------------------------------------------------------------------------------------------- 
	
	//RECORD LOCK
	
	int fd_lock;
    struct flock region_lock;

	
    //NOTA SE HAI OCREAT E OEXCL E IL FILE ESISTE GIA, LA OPEN FALLISCE
    fd_lock = open("lock_file", O_RDWR | O_CREAT , 0666);
    if (!fd_lock) {
        fprintf(stderr, "Unable to open lockfile for read/write\n");
        exit(EXIT_FAILURE);
    }

	//settiamo il lock
    region_lock.l_whence = SEEK_SET;
    region_lock.l_start = 0;
    region_lock.l_len = 8;

	//lock region
	region_lock.l_type = F_WRLCK; //si setta a lettura e scrittura
    if (fcntl(fd_lock, F_SETLKW, &region_lock) == -1){
    	fprintf(stderr, "Failed to lock region 1\n");
    }else{
		printf("Process %d locked file\n", getpid());
	}
	
	//unlock region	
	region_lock.l_type = F_UNLCK; //SI SETTA A UNLUCK
   	if (fcntl(fd_lock, F_SETLKW, &region_lock) == -1){
    	fprintf(stderr, "Failed to lock region 1\n");
	}else{
		printf("Process %d unlocked file\n", getpid());
	}	
	
	
//----------------------------------------------------------------------------------------------- 

	//PIPE
	
	// Si legge sempre da 0 e si scrive su 1
	
	int fd_pipe[2];
	pid_t pid_child;
    
	/*
	Questo è il codice per disattivare il close on exec flag
	NOTA: se fai dup2 non serve disattivarlo perche tanto duplichi il fd ma il flag rimane quello di fdin, fdout o quel che è, che ha gia il bit non settato
	Lo usi se vuoi che parent e child comunichino tramite pipe, mantenendo fdin e fdout
	Il child, se non apre altri file descriptor, potrà leggere sul 3 e scrivere sul 4 
	*/
	
	//fcntl(fd[0], F_SETFD, fcntl(fd[0], F_GETFD) & ~FD_CLOEXEC);
    //fcntl(fd[1], F_SETFD, fcntl(fd[1], F_GETFD) & ~FD_CLOEXEC);
    
	
	if (pipe(fd_pipe) < 0){
		printf("Errore nel creare la pipe");	
	}
	
	if ((pid_child = fork()) < 0){
		printf("Errore nella fork.");
	}else if (pid_child > 0){ //PARENT

		close(fd_pipe[0]); // in genere il parent scrive al child, allora può chiudere il fd di lettura
	}else if (pid_child == 0){ //CHILD

		close(fd_pipe[1]); //in genere il child legge, quindi possiamo chiudere fd di scrittura	
		execl("eseguibile", "nomeprocesso", (char *) NULL); //elenchi i parametri da passare, mettendo (void*) NULL quando hai finito l'elenco 
	}
	
	int status;
	if ((pid = waitpid(pid, &status, 0)) < 0)
		printf("waitpid error");

	
	
//----------------------------------------------------------------------------------------------- 
// CREATING A NAMED PIPE, A ->FIFO<-
    int fd;

    // Create the named pipe
    mkfifo("my_fifo", 0666); <- you need to create it once of course, then you just open it



    // Open the named pipe for writing
    fd = open("my_fifo", O_RDWR);

    // Write some data to the pipe
    write(fd, "Hello, world!", 14);


    char buffer[100];

    read(fd, buffer, 14);
    printf("%s", buffer);

    close(fd);

//----------------------------------------------------------------------------------------------- 
 	//POPEN
 	
 	  
  	FILE *fp_popen;
  	char line[256];
	
	//la popen fa una fork ed exec del programma passato come argomento
	//se "r" mi ritorna un file pointer all'output che leggiamo come un file 		
  	fp_popen = popen("comando in bash", "r");
  	if (fp_popen == NULL) {
    	printf("Failed to run command\n" );
    	return 1;
  	}

	//leggo il "file" output e lo stampo
  	while (fgets(line, sizeof(line), fp_popen) != NULL) {
    	printf("%s", line);
  	}

  	pclose(fp_popen);
  	
  	//se invece vogliamo inviare noi l'input a un programma lanciato con popen, si usa modalità "w"
  	//in questo caso ci viene ritornato un file pointer in cui scriviamo l'input, e quando chiudiamo il fp, viene mandato in input al comando specificato 
  	fp_output = popen("comando in bash", "w");
  	if (fp_output == NULL) {
    	printf("Failed to run command\n");
    	return -1;
  	}
  	
  	//scrivi su fp_output
  	
  	pclose(fp_output); // si chiude il file pointer su cui noi scriviamo l'input del programma, allora viene avviato il programma 
  			

//----------------------------------------------------------------------------------------------- 


      		
	int maxFD = 10;
	fd_set tempset, masterset;
	
	FD_ZERO(&masterset);
 	FD_SET(sockfd_1, &masterset);
 	FD_SET(sockfd_2, &masterset);

    
  	int quit = 0;
  	while(!quit)
  	{
  		tempset = masterset;
        select(maxFD, &tempset, NULL, NULL, NULL);
                
        printf("... received something... ");

		if (FD_ISSET(sockfd_1, &tempset))
		{
        }
		if (FD_ISSET(sockfd_2, &tempset))
		{
        }
    
    }
    



//----------------------------------------------------------------------------------------------- 
//-----------------------------------------------------------------------------------------------

//element for non canonical terminal
struct termios old, new;

void not_canonical_terminal_set(){
    /* Get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO, &old);
    /* Copy the old settings to new settings */
    new = old;
    /* Make changes to the new settings */
    new.c_lflag &= ~(ICANON | ECHO);
    /* Apply the new settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void reset_canonical_terminal(){
	/* Reapply the old settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}

void get_inupt_not_canonical(){
	
	//queste due righe sono per rendere non bloccante la read da stdin, se leggi leggi altrimenti vai avanti 
	int flags = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, flags | O_NONBLOCK);

	//altrimenti leggi intera stringa
	char c;
	if (read(0, &c, 1) > 0)
    {
        printf("letto da stdin: %c\n", c);
        fflush(stdout);
    }
    
    
	//c = getchar(); 
	fflush(stdout);
	
	//fcntl(0, F_SETFL, flags); //rendere di nuovo bloccante la read

}

int main() {

	not_canonical_terminal_set();
	get_inupt_not_canonical();
	reset_canonical_terminal();

	return 0;
}





//----------------------------------------------------------------------------------------------- 
//-----------------------------------------------------------------------------------------------


  //GENERA STRINGA CASUALE
  
//nel main bisognerebbe usare   srand(time(NULL)); // seed the random number generator

void random_string(char *str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
}



//----------------------------------------------------------------------------------------------- 

 	//PROFILAZIONE DI UN PROCESSO 
 	
 	struct tms tmsstart, tmsend;
	clock_t start, end;
 	
 	if ((start = times(&tmsstart)) == -1)	//inizio 
		printf("Errore nel calcolo dei tempi");	
		
	if ((end = times(&tmsend)) == -1)	//fine
		printf("Errore nel calcolo dei tempi");	
	
	pr_times(end-start, &tmsstart, &tmsend); //stampa valori di differenza
	
	
 	static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
	{
		static long		clktck = 0;

		if (clktck == 0)	/* fetch clock ticks per second first time */
			if ((clktck = sysconf(_SC_CLK_TCK)) < 0)
				printf("errore in prtimes");
		
		printf("  real:  %7.2f\n", real / (double) clktck);
		printf("  user:  %7.2f\n", (tmsend->tms_utime - tmsstart->tms_utime) / (double) clktck);
		printf("  sys:   %7.2f\n", (tmsend->tms_stime - tmsstart->tms_stime) / (double) clktck);
		printf("  child user:  %7.2f\n", (tmsend->tms_cutime - tmsstart->tms_cutime) / (double) clktck);
		printf("  child sys:   %7.2f\n", (tmsend->tms_cstime - tmsstart->tms_cstime) / (double) clktck);

	}
	
	 
	
	
	//--------------------------------
	
		
	//per estrarre l'orario in secondi dalla epoch, e poi trasformarlo in stringa. vedi screen con i tempi per capire meglio come muoverti
	time_t current_time = time(NULL);
	struct tm *time_info = localtime(&current_time);
	char temp_time[100];
	strftime(temp_time, 100, "%Y-%m-%d %H:%M:%S", time_info);
	printf("Time: %s\n", temp_time);

	


	//------------------------------------------------------------------------------------------
	//OPERAZIONI SU UN FILE
	
	//apri file il append, se non esiste lo crea
	//per controllare modalità 
	//https://www.programiz.com/c-programming/c-file-input-output#:~:text=The%20syntax%20for%20opening%20a,bin%22%2C%22rb%22)%3B
	FILE *fptr;
	
	fptr = fopen("file.txt","a+"); //r+ è reading e writing, la b significa binary mode
	
	if(fptr == NULL)
	{
      printf("Error!");   
      exit(1);             
	}
	


	char line[64];
	int count = 0;
	int lineNumber = 1;
	
	fseek(fptr, 0, SEEK_SET); <- IMPORTANTISSIMA //riposiziona all'inizio perche dopo aver scritto sarà posizionato alla fine
	
	while (fgets(line, sizeof(line), fptr) != NULL) /* read a line */
    {
        if (count == lineNumber) //leggi le righe fino a quella richiesta
        {
            printf("%s\n", line);
			break;
        }
        else
        {
            count++;
        }        
           
    }
    
	fclose(fptr); 
	
	//--------------------------------------------------------------------------
	
	//IN ALTERNATIVA CON LA OPEN
	
	int fd_file = open("file1.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
	char writebuf[128] = "primariga\nseconda riga\ne terzaaa\0";
	
	write(fd_file, writebuf, strlen(writebuf));
	
	lseek(fd_file, 0, SEEK_SET); <- IMPORTANTISSIMA
	
	char readbuf[128];
	read(fd_file, readbuf, 128);
	printf("Letto dal file \n%s\n", readbuf);
	
	
	
	//----------------------------------------------------------------------------------------------------
	    
	    

	//LEGGERE DA STDIN
	char buf[64];
	fgets(buf, 64, stdin); //NOTA CHE SI AGGIUNGE ANCHE \n ALLA FINE DELL'INPUT
	buf[strlen(buf) - 1] = 0;


	//TRASFORMARE NUMERO IN STRINGA (OPPOSTO DI atoi())
	int number = 42;
    char string[10];
    sprintf(string, "%d", number);
    
    

	//DIVIDERE STRINGA IN TOKEN
	//dove buf è la stringa, e ogni strtok si divide la stringa una volta in piu. se sai che sono due pezzi basta farla una volta, se non sai quanti sono fai un ciclo come segue.
	char * token = strtok(buf, " ");
   	while( token != NULL ) {
    	printf( " %s\n", token ); //printing each token
      	token = strtok(NULL, " ");
      		}
	
	
	