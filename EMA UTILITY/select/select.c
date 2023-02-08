#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
In questo esempio, vengono aperti due file, file1.txt e file2.txt, e i relativi 
descrittori di file vengono aggiunti al set dei descrittori. La funzione select() 
attende fino a 10 secondi per l'input su entrambi i descrittori. Se l'input è disponibile 
su uno o entrambi i descrittori, viene stampato un messaggio indicante quale file ha generato 
l'input. In caso contrario, viene stampato un messaggio di timeout.


Quando si utilizza la funzione select() per controllare più descrittori di file, 
la funzione attende fino a quando almeno uno dei descrittori specificati è pronto per 
l'operazione specificata (in questo caso, la lettura). Nel caso specifico dell'esempio che 
ho fornito, la funzione select() attende fino a quando uno dei due file, file1.txt o file2.txt , 
ha qualcosa da leggere, e quando uno dei due file ha qualcosa da leggere, la funzione select() restituisce
 indicando che quel descrittore è pronto per la lettura.

In altre parole, quando si utilizza select() per controllare più descrittori, la funzione non 
attende che tutti i descrittori siano pronti, ma attende fino a quando almeno uno di essi lo è.
 In questo modo è possibile gestire più descrittori contemporaneamente e gestire l'input da più 
 fonti contemporaneamente.
 */

int main() {
    fd_set readfds;
    struct timeval timeout;
    int ret, fd1, fd2;

    // Open two files
    fd1 = open("file1.txt", O_RDONLY);
    fd2 = open("file2.txt", O_RDONLY);

    // Clear the descriptor set
    FD_ZERO(&readfds);

    // Add the file descriptors to the descriptor set
    FD_SET(fd1, &readfds);
    FD_SET(fd2, &readfds);

    // Set the timeout value
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    // Wait for input on the file descriptors for up to 10 seconds
    ret = select(fd2 + 1, &readfds, NULL, NULL, &timeout);

    if (ret == -1) {
        perror("select()");
    } else if (ret) {
        if (FD_ISSET(fd1, &readfds)) {
            // input is available on fd1
            printf("Input available on file1.txt\n");
        }
        if (FD_ISSET(fd2, &readfds)) {
            // input is available on fd2
            printf("Input available on file2.txt\n");
        }
    } else {
        printf("Timeout occured\n");
    }

    close(fd1);
    close(fd2);
    return 0;
}
