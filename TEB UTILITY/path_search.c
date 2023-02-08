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

static char	*fullpath;		/* contains full pathname for every file */
#define PATHLEN 512
//#define NAME_MAX 64;


static int myfunc(const char *pathname, const struct stat *statptr, int type);

static int	dopath();

static long	nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char *argv[])
{

	if(argc != 2)
		printf("Inserisci l'argomento.\n");
		
	fullpath = malloc(PATHLEN); //supponendo una lunghezza massima per un percorso di 256 caratteri
	strcpy(fullpath, argv[1]);
	dopath();
	

	
	//CALCOLO LA PERCENTUALE DI PRESENZA DI OGNI TIPO DI FILE 
	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
	if (ntot == 0) 
		ntot = 1;		// avoid divide by 0; print 0 for all counts 
	printf("regular files  = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
	printf("directories    = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);
	printf("block special  = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);
	printf("char special   = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
	printf("FIFOs          = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);
	printf("symbolic links = %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);
	printf("sockets        = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);
	return 0;
}



/*
 * Descend through the hierarchy, starting at "fullpath".
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return.  For a directory, we call ourself
 * recursively for each name in the directory.
 */
 
static int dopath()
{
	struct stat		statbuf;
	struct dirent	*dirp;
	DIR				*dp;
	int				ret, n;

	if (lstat(fullpath, &statbuf) < 0)	/* stat error */
		return(myfunc(fullpath, &statbuf, FTW_NS)); //its a file that we can't do stat
		
	if (S_ISDIR(statbuf.st_mode) == 0)	
		return(myfunc(fullpath, &statbuf, FTW_F)); /* not a directory */

	/*
	 * It's a directory.  First call func() for the directory,
	 * then process each filename in the directory.
	 */
	 
	if ((ret = myfunc(fullpath, &statbuf, FTW_D)) != 0) /*call my func for the directory*/
		return(ret);

	n = strlen(fullpath);
	fullpath[n] = '/'; //prepariamo il path ad accogliere nuovi file
	fullpath[++n] = 0; //attuale endline

	if ((dp = opendir(fullpath)) == NULL)	
		return(myfunc(fullpath, &statbuf, FTW_DNR));  /* can't read directory */

	while ((dirp = readdir(dp)) != NULL) { 
		if (strcmp(dirp->d_name, ".") == 0  || strcmp(dirp->d_name, "..") == 0)
				continue;		/* ignore dot and dot-dot */
				
		strcpy(&fullpath[n], dirp->d_name);	/* append name after "/" */
		if ((ret = dopath(myfunc)) != 0)		/* recursive */
			break;	/* time to leave */       
	}
	
	fullpath[n-1] = 0;	/* erase everything from slash onward */
	
	if (closedir(dp) < 0){
		printf("Can't close directory %s\n", fullpath);
		return -1;
		}
	return(ret);
}

static int myfunc(const char *pathname, const struct stat *statptr, int type)
{
	switch (type) {
	case FTW_F:
		switch (statptr->st_mode & S_IFMT) {
		case S_IFREG:	nreg++;		break;
		case S_IFBLK:	nblk++;		break;
		case S_IFCHR:	nchr++;		break;
		case S_IFIFO:	nfifo++;	break;
		case S_IFLNK:	nslink++;	break;
		case S_IFSOCK:	nsock++;	break;
		//case S_IFDIR:	/* directories should have type = FTW_D */
			//err_dump("for S_IFDIR for %s", pathname);
		}
		break;
	case FTW_D:
		ndir++;
		break;
	//case FTW_DNR:
		//err_ret("can't read directory %s", pathname);
		//break;
	//case FTW_NS:
		//err_ret("stat error for %s", pathname);
		//break;
	//default:
		//err_dump("unknown type %d for pathname %s", type, pathname);
	}
	return(0);
}
