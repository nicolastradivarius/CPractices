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

#define THREAD_NUM 4
#define LIMIT 5

sem_t sem_A1, sem_B1, sem_C, sem_D, sem_A2, sem_B2;
pthread_t threads[THREAD_NUM];

/*
Generar la secuencia ABCABD... usando semáforos.
*/

void *process_A() {
    for (int i=0; i<LIMIT; i++) {

        // primera "A"
        sem_wait(&sem_A1);
        printf("A");
        fflush(stdout);
        sem_post(&sem_B1);

        // segunda "A"
        sem_wait(&sem_A2);
        printf("A");
        fflush(stdout);
        sem_post(&sem_B2);
    }
    return NULL;
}

void *process_B() {
    for (int i=0; i<LIMIT; i++) {
        sem_wait(&sem_B1);
        printf("B");
        fflush(stdout);
        sem_post(&sem_C);

        sem_wait(&sem_B2);
        printf("B");
        fflush(stdout);
        sem_post(&sem_D);
    }
    return NULL;
}

void *process_C() {
    for (int i=0; i<LIMIT; i++) {
        sem_wait(&sem_C);
        printf("C");
        fflush(stdout);
        sem_post(&sem_A2);
    }
    return NULL;
}

void *process_D() {
    for (int i=0; i<LIMIT; i++) {
        sem_wait(&sem_D);
        printf("D");
        fflush(stdout);
        sem_post(&sem_A1);
    }
    return NULL;
}

void main() {
    sem_init(&sem_A1, 0, 1);
    sem_init(&sem_B1, 0, 0);
    sem_init(&sem_C, 0, 0);
    sem_init(&sem_D, 0, 0);
    sem_init(&sem_A2, 0, 0);
    sem_init(&sem_B2, 0, 0);

    pthread_create(&threads[0], NULL, &process_A, NULL);
    pthread_create(&threads[1], NULL, &process_B, NULL);
    pthread_create(&threads[2], NULL, &process_C, NULL);
    pthread_create(&threads[3], NULL, &process_D, NULL);

    for (int i=0; i<THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n");

    sem_destroy(&sem_A1);
    sem_destroy(&sem_B1);
    sem_destroy(&sem_C);
    sem_destroy(&sem_D);
    sem_destroy(&sem_A2);
    sem_destroy(&sem_B2);

    exit(0);
}