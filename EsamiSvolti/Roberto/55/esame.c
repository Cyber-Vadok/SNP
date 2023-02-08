/*
Build a C program that takes a directory path as an argument and has two threads:
1) thread1 takes the argument and find all the regular files whose path is rooted in the argument and have a link count greater than 1.
2) thread2 takes each found file and looks for files with the same inode of the found file in the same hierarchy where the search was conducted.
Have the two threads properly synchronized.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>


/*
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */
#define	FTW_F	1		/* file other than directory */
#define	FTW_D	2		/* directory */
#define	FTW_DNR	3		/* directory that can't be read */
#define	FTW_NS	4		/* file that we can't stat */
#define PATHLEN 512

pthread_t t1;  
static char	*fullpath;		/* contains full pathname for every file */

static void * thread1Func(void *arg);

int main(int argc, char *argv[]){

  if(argc != 2){
    printf("Inserisci l'argomento.\n");
    exit(1);
  }
  

  fullpath = malloc(PATHLEN); //supponendo una lunghezza massima per un percorso di 256 caratteri
  strcpy(fullpath, argv[1]);

  if (pthread_create(&t1, NULL, thread1Func, NULL) != 0)
      printf("Errore nella creazione della thread");    
	
  if (pthread_join(t1, NULL) != 0)
      printf("Errore nella join del thread");    

  if (pthread_cancel(t1) != 0)
	printf("Errore nel cancellare la thread t1");
}



/*
 * Descend through the hierarchy, starting at "fullpath".
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return.  For a directory, we call ourself
 * recursively for each name in the directory.
 */
 

static void * thread1Func(void *arg){
  struct stat		statbuf;
  struct dirent	        *dirp;
  DIR		        *dp;
  int			ret, n;
  
  n = strlen(fullpath);
  fullpath[n] = '/'; //prepariamo il path ad accogliere nuovi file
  fullpath[++n] = 0; //attuale endline
  
  if ((dp = opendir(fullpath)) == NULL)	
    printf("non riesco a leggere la cartella");  /* can't read directory */

  while ((dirp = readdir(dp)) != NULL) { 
    if (strcmp(dirp->d_name, ".") == 0  || strcmp(dirp->d_name, "..") == 0)
		  continue;		/* ignore dot and dot-dot */
		  
    //strcpy(&fullpath[n], dirp->d_name);	/* append name after "/" */
    
    if (statbuf->st_mode & S_IFMT == S_IFREG && statbuf->st_nlink > 1){
      printf("il file %s ha piu' di un link\n", dirp->d_name);
    }  
    
  } 
}

  fullpath[n-1] = 0;	/* erase everything from slash onward */

  if (closedir(dp) < 0){
  printf("Can't close directory %s\n", fullpath);
  return -1;
  }
  return(ret);
}
