#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>

//FILE *logfile;

void
daemonize(const char *cmd)
{
	int					i, fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		printf("%s: can't get file limit", cmd);

	/*
	 * Become a session leader to lose controlling TTY.
	 */


	if ((pid = fork()) < 0)
		printf("%s: can't fork", cmd);
	if (pid != 0){ /* parent */
		exit(0);
		}
		
	setsid();
	

	/*
	 * Ensure future opens won't allocate controlling TTYs.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		printf("%s: can't ignore SIGHUP", cmd);
	if ((pid = fork()) < 0){
		printf("%s: can't fork", cmd);
	}else if (pid != 0){ /* parent */
		exit(0);
		}
		
	printf("cuai");
	fflush(stdout);

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
	if (chdir("/") < 0)
		printf("%s: can't change directory to /", cmd);

	/*
	 * Close all open file descriptors.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/*
	 * Attach file descriptors 0, 1, and 2 to /dev/null.
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/*
	 * Initialize the log file.
	 */
	openlog(cmd, LOG_PID | LOG_CONS, LOG_DAEMON); //invece di LOG_USER c'era LOG_DAEMON
	syslog(LOG_NOTICE, "Il processo è stato daemonizzato.");        

	
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
		  fd0, fd1, fd2);
		exit(1);
	}
	
	
	
	//ALTRIMENTI CON UN FILE NORMALE 
	/*
	logfile = fopen("/Users/francescodenu/Desktop/SYSTEM_NETWORK_PROGRAMMING/my_ESAMI/11/mydaemonlog.txt", "w");
    if (logfile == NULL) {
        perror("Error opening log file");
        exit(1);
    }

    // Write log messages to the file
	fprintf(logfile, "Daemon started\n");
	fflush(logfile);
	*/
	
}


int main(int argc, char* argv[]) 
{
	
	/*
	NOTA IMPORTANTE:
	per leggere i file di log in teoria su mac dovresti usare il comando tail -f /var/log/system.log 
	In pratica per qualche motivo non funziona, e si usa: 
	log show --predicate 'process == "my_daemon"' (non so se vuole il nome del processo, del logfile o cosa, ma nel mio caso è tutto uguale)
	*/
	daemonize(argv[0]);
	
	syslog(LOG_NOTICE, "Il processo è terminato.");        
	closelog();
    
    return 0;
}

