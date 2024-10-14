#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

#define PROCESS_NUM 2
#define LIMIT 5

/**
 * Problema: Generar la secuencia ABC(EoF)ABD(EoF)... usando procesos, y donde la comunicación y sincronización sea con pipes.
 * La idea básica será tener un proceso principal que cree otros 5 procesos hijos y que se comuniquen y sincronicen a través de pipes. Por ejemplo, el proceso que imprime B debe esperar a que le llegue por el pipe la confirmación del proceso A.
 * 
 * Podemos aprovecharnos de que cuando un pipe está vacío, el proceso lector se bloquea hasta que el escritor escribe algo. Similarmente, cuando el pipe está lleno, el proceso escritor se bloquea hasta que el lector lee algo.
 */

void main() {
    // Primero vamos a hacerlo con dos procesos para entender la idea:

    // dos pipes, uno para ir del proceso A al B y otro de B a A, teniendo en cuenta que el proceso principal es el proceso B, que debe esperar a que el hijo haga "A".
    int pfd1[2], pfd2[2];
    pipe(pfd1); // pipe para que A escriba y B lea
    pipe(pfd2); // pipe para que B escriba y A lea

    pid_t pid = fork();

    if (pid == 0) {
        // proceso A

        // cierro los extremos que no voy a usar
        close(pfd1[0]);
        close(pfd2[1]);

        for (int i=0; i<LIMIT; i++) {
            write(pfd1[1], "A", 1); 
            char c;
            read(pfd2[0], &c, 1); // me bloqueo hasta que llegue una B
            printf("%c", c);
            fflush(stdout);
        }

        exit(0);
    } else {
        // proceso B

        // cierro los extremos que no voy a usar
        close(pfd1[1]);
        close(pfd2[0]);

        for (int i=0; i<LIMIT; i++) {
            char c;
            read(pfd1[0], &c, 1); 
            printf("%c", c);
            fflush(stdout);
            write(pfd2[1], "B", 1); // le aviso al proceso A que ya imprimí
        }
    }

    printf("\n");

    wait(NULL);
    exit(0);
}