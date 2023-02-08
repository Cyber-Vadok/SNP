#include <time.h>
#include <stdio.h>

int main() {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // esegui l'azione di cui vuoi misurare il tempo qui
    // ad esempio, un ciclo di elaborazione
    for (int i = 0; i < 100000000; i++);

    clock_gettime(CLOCK_MONOTONIC, &end);

    // calcola la differenza tra le due misure
    long diff_sec = end.tv_sec - start.tv_sec;
    long diff_nsec = end.tv_nsec - start.tv_nsec;
    double elapsed = diff_sec + (double)diff_nsec / 1000000000;

    printf("Tempo trascorso: %fs\n", elapsed);

    return 0;
}
