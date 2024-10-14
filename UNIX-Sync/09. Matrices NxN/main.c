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
 * - El proceso padre forkeara N procesos.
 * - Cada uno de estos procesos hijos se encargará de multiplicar una fila de la matriz por la columna de la otra matriz. El proceso P1 multiplica la fila 1 de A por la columna 1 de B, el proceso P2 la fila 2 de A por la columna 2 de B, y así sucesivamente.
 * - Con los semáforos sincronizaremos para que los procesos impriman la matriz resultado en orden, de arriba hacia abajo (en filas)
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

sem_t *sem_P1, *sem_P2, *sem_P3, *sem_P4, *sem_P5;
sem_t *mutex;
struct Matrices *matrices;

int main() {
    if (sem_unlink("/mymutex") == -1 || sem_unlink("/mysem1") == -1 || sem_unlink("/mysem2") == -1 || sem_unlink("/mysem3") == -1 || sem_unlink("/mysem4") == -1 || sem_unlink("/mysem5") == -1) {
        // do nothing
    } else {
        printf("Semaphore unlinked successfully.\n");
    }
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

    mutex = sem_open("/mymutex", O_CREAT | O_EXCL, 0666, 1);
    sem_P1 = sem_open("/mysem1", O_CREAT | O_EXCL, 0666, 1);
    sem_P2 = sem_open("/mysem2", O_CREAT | O_EXCL, 0666, 0);
    sem_P3 = sem_open("/mysem3", O_CREAT | O_EXCL, 0666, 0);
    sem_P4 = sem_open("/mysem4", O_CREAT | O_EXCL, 0666, 0);
    sem_P5 = sem_open("/mysem5", O_CREAT | O_EXCL, 0666, 0);

    if (sem_P1 == SEM_FAILED || sem_P2 == SEM_FAILED || sem_P3 == SEM_FAILED || sem_P4 == SEM_FAILED || sem_P5 == SEM_FAILED || mutex == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    srand(time(NULL));
    // inicializar las matrices A y B (se podría paralelizar)
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
    // y luego esperará para mostrarlas.
    printf("Matriz C:\n");
    fflush(stdout);
    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    sem_wait(mutex);
                    matrices->matrizC[i][j] += matrices->matrizA[i][k] * matrices->matrizB[k][j];
                    sem_post(mutex);
                }
            }

            if (i == 0) {
                sem_wait(sem_P1);
                // muestra la primer fila de la matriz C
                for (int j = 0; j < N; j++) {
                    printf("%d ", matrices->matrizC[i][j]);
                    fflush(stdout);
                }
                printf("\n");
                sem_post(sem_P2);
            }

            if (i == 1) {
                sem_wait(sem_P2);
                // muestra la segunda fila de la matriz C
                for (int j = 0; j < N; j++) {
                    printf("%d ", matrices->matrizC[i][j]);
                    fflush(stdout);
                }
                printf("\n");
                sem_post(sem_P3);
            }

            if (i == 2) {
                sem_wait(sem_P3);
                // muestra la tercer fila de la matriz C
                for (int j = 0; j < N; j++) {
                    printf("%d ", matrices->matrizC[i][j]);
                    fflush(stdout);
                }
                printf("\n");
                sem_post(sem_P4);
            }

            if (i == 3) {
                sem_wait(sem_P4);
                // muestra la cuarta fila de la matriz C
                for (int j = 0; j < N; j++) {
                    printf("%d ", matrices->matrizC[i][j]);
                    fflush(stdout);
                }
                printf("\n");
                sem_post(sem_P5);
            }

            if (i == 4) {
                sem_wait(sem_P5);
                // muestra la quinta fila de la matriz C
                for (int j = 0; j < N; j++) {
                    printf("%d ", matrices->matrizC[i][j]);
                    fflush(stdout);
                }
                printf("\n");
            }

            exit(0);
        }
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }
/*
    printf("Matriz C:\n");
    for (int i = 0; i < N; i++) {
        sem_wait(sem_P1);
        for (int j = 0; j < N; j++) {
            printf("%d ", matrices->matrizC[i][j]);
            fflush(stdout);
        }
        printf("\n");
    }
*/



    sem_close(sem_P1);
    sem_close(sem_P2);
    sem_close(sem_P3);
    sem_close(sem_P4);
    sem_close(sem_P5);
    sem_close(mutex);

        sem_unlink("/mysem1");
    sem_unlink("/mysem2");
    sem_unlink("/mysem3");
    sem_unlink("/mysem4");
    sem_unlink("/mysem5");
    sem_unlink("/mymutex");

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