#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int sigint_handler(int signum) {
    printf("\nCaught SIGINT %d! Exiting...\n",signum);
    exit(0);
}

int main() {
    signal(SIGINT, sigint_handler);
    while (1) {
        // Do some work
    }
    return 0;
}