/**
 * Resuelva el problema de multiplicacion de dos matrices de NxN.
 * La solucion debe contemplar el uso de procesos y segmento de memoria compartida.
 * Utilice semáforos para que los resultados se muestren de forma ordenada (P1, P2, ..., P5).
 * Considere N con valor 5.
 * (o sea, multiplicar dos matrices de 5x5).
 * 
 * La idea general sería:
 * - Tener un proceso padre que deberá crear la sección de memoria compartida.
 * - En la memoria compartida habrán 2 matrices, las de entrada A y B.
 * - El proceso padre forkeara N procesos y en cada uno, llamará a exec con los siguientes parámetros:
 * - Cada uno de estos procesos hijos se encargará de multiplicar una fila de la matriz por la columna de la otra matriz. El proceso P1 multiplica la fila 1 de A por la columna 1 de B, el proceso P2 la fila 2 de A por la columna 2 de B, y así sucesivamente.
 * - Con los semáforos sincronizaremos para que los procesos impriman la matriz resultado en orden.
 * - Cada proceso imprimirá una fila de la matriz resultado, empezando de arriba hacia abajo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

#define N 5
#define KEY ((key_t) (1255))

struct Matrices {
    int matrizA[N][N];
    int matrizB[N][N];
    int matrizC[N][N];
};

#define SEGSIZE sizeof(struct Matrices)

sem_t *sem;
struct Matrices *matrices;

int main() {
    int shmid;

    shmid = shmget(KEY, SEGSIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    matrices = (struct Matrices *)shmat(shmid, NULL, 0);
    if (matrices == (struct Matrices *)-1) {
        perror("shmat");
        exit(1);
    }

    sem = sem_open("/mysem", O_CREAT | O_EXCL, 0666, 0);

    srand(time(NULL));
    // inicializar las matrices A y B
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrices->matrizA[i][j] = i*j;
            matrices->matrizB[i][j] = i+j;
        }
    }

    // inicializar la matriz C.
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrices->matrizC[i][j] = 0;
        }
    }

    // mostrar por consola las matrices A y B
    printf("Matriz A:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matrices->matrizA[i][j]);
            fflush(stdout);
        }
        printf("\n");
    }

    printf("Matriz B:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matrices->matrizB[i][j]);
            fflush(stdout);
        }
        printf("\n");
    }

    // cada proceso hijo se encargará de multiplicar una fila de la matriz A por la columna de la matriz B
    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    matrices->matrizC[i][j] += matrices->matrizA[i][k] * matrices->matrizB[k][j];
                }
            }
            sem_post(sem);
            exit(0);
        }
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }

    printf("Matriz C:\n");
    for (int i = 0; i < N; i++) {
        sem_wait(sem);
        for (int j = 0; j < N; j++) {
            printf("%d ", matrices->matrizC[i][j]);
            fflush(stdout);
        }
        printf("\n");
    }
    sem_close(sem);
    sem_unlink("/mysem");

    if (shmdt(matrices) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(1);
    }

    //borrar los segmentos de memoria
    shmctl(shmid, IPC_RMID, 0);

    return 0;
}