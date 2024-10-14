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

#define PROCESS_NUM 5
#define LIMIT 5

/**
 * Problema: Generar la secuencia ABC(EoF)ABD(EoF)... usando procesos, y donde la comunicación y sincronización sea con pipes.
 * 
 * Podemos aprovecharnos de que cuando un pipe está vacío, el proceso lector se bloquea hasta que el escritor escribe algo. Similarmente, cuando el pipe está lleno, el proceso escritor se bloquea hasta que el lector lee algo.
 * 
 * La idea básica es que tenemos dos pipes: uno para que el padre se comunique con sus hijos, y otro para viceversa.
 * Con estos pipes, los hijos se quedarán bloqueados esperando a que el padre les dé la orden de imprimir su letra. 
 * A su vez, el padre también se quedará bloqueado hasta que los hijos le informen que terminaron de imprimir su letra.
 * La primer letra que debe imprimirse es la A, pero también hay que tener en cuenta que esta letra debe volver a imprimirse después que se imprima la (EoF); por lo tanto, el padre debe iniciar la secuencia indicándole al proceso A, pero luego debe esperar a que EoF le indique que puede volver a indicarle al proceso A que puede imprimir la A.
 * 
 */

void imprimir_letra(int fd_read, int fd_write);

int main() {
    srand(time(NULL));
    // creamos dos pipes bidimensionales, donde en cada índice hay un pipe.
    int pipe1[PROCESS_NUM][2]; // pipes para comunicación entre el proceso padre y los hijos
    int pipe2[PROCESS_NUM][2]; // pipes para comunicación entre los hijos y el padre
    char c;

    for (int i=0; i<PROCESS_NUM; i++) {
        pipe(pipe1[i]);
        pipe(pipe2[i]);
        pid_t pid = fork();
        if (pid > 0) {
            // el proceso padre debe cerrar los extremos que no va a usar
            close(pipe1[i][0]);
            close(pipe2[i][1]);
        } else if (pid == 0) {
            // los hijos deben cerrar los extremos que no van a usar
            close(pipe1[i][1]);
            close(pipe2[i][0]);

            imprimir_letra(pipe1[i][0], pipe2[i][1]);
        } else {
            perror("Error al crear el proceso hijo");
            exit(-1);
        }
    }

    for (int j=0; j<LIMIT; j++) {
        c = 'A';
        write(pipe1[0][1], &c, sizeof(char)); // le envío la primera letra al proceso A
        read(pipe2[0][0], &c, sizeof(char)); // espero a que el proceso A me confirme que imprimió

        // le envío la letra B al proceso B
        c = 'B';
        write(pipe1[1][1], &c, sizeof(char));
        read(pipe2[1][0], &c, sizeof(char)); // espero a que el proceso B me confirme que imprimió

        // le envío la letra C al proceso C
        c = 'C';
        write(pipe1[2][1], &c, sizeof(char));
        read(pipe2[2][0], &c, sizeof(char)); // espero a que el proceso C me confirme que imprimió

        //le envío la letra E al proceso E
        c = 'E';
        write(pipe1[4][1], &c, sizeof(char));
        read(pipe2[4][0], &c, sizeof(char)); // espero a que el proceso E me confirme que imprimió

        // le envío la letra A al proceso A
        c = 'A';
        write(pipe1[0][1], &c, sizeof(char)); // le envío la primera letra al proceso A
        read(pipe2[0][0], &c, sizeof(char)); // espero a que el proceso A me confirme que imprimió

        // le envío la letra B al proceso B
        c = 'B';
        write(pipe1[1][1], &c, sizeof(char));
        read(pipe2[1][0], &c, sizeof(char)); // espero a que el proceso B me confirme que imprimió

        // le envío la letra D al proceso D
        c = 'D';
        write(pipe1[3][1], &c, sizeof(char));
        read(pipe2[3][0], &c, sizeof(char)); // espero a que el proceso D me confirme que imprimió

        // le envío la letra E al proceso E
        c = 'E';
        write(pipe1[4][1], &c, sizeof(char));
        read(pipe2[4][0], &c, sizeof(char)); // espero a que el proceso E me confirme que imprimió

        printf("_");
        fflush(stdout);
    }

    for (int i=0; i<PROCESS_NUM; i++) {
        write(pipe1[i][1], "X", sizeof(char)); // le envío la señal de terminación a los hijos
        wait(NULL);
    }

    fflush(stdout);
    printf("\n");
    fflush(stdout);

    exit(0);
}

void imprimir_letra(int fd_read, int fd_write) {
    while (1) {
        char c;
        read(fd_read, &c, sizeof(char));
        if (c == 'X') {
            exit(0);
        }
        if (c == 'E') {
            c = random() % 2 == 0 ? 'E' : 'F';
        }
        printf("%c", c);
        fflush(stdout);
        write(fd_write, &c, sizeof(char));
    }
    
}