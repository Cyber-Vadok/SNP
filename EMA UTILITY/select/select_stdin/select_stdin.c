#include <stdio.h>
#include <sys/select.h>

int main() {
    fd_set set;
    struct timeval timeout;
    int rv;
    char buffer[1024];

    // Inizializziamo la struttura fd_set
    FD_ZERO(&set);
    FD_SET(0, &set);

    // Impostiamo un timeout di 5 secondi
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // Eseguiamo la select
    rv = select(1, &set, NULL, NULL, &timeout);

    // Controlliamo il valore di ritorno
    if(rv == -1) {
        perror("select"); // errore
    } else if(rv == 0) {
        printf("timeout\n"); // nessun input entro il timeout
    } else {
        // stdin pronto per la lettura
        fgets(buffer, sizeof(buffer), stdin);
        printf("input: %s", buffer);
    }

    return 0;
}
