#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

#define THREAD_NUM 3
#define LIMIT 5

// secuencia "abcab" con tres hilos

sem_t sem_A1, sem_B1, sem_C, sem_A2, sem_B2;

void *process_A() {
    for (int i=0; i<LIMIT; i++) {
        sem_wait(&sem_A1);
        printf("A");
        fflush(stdout);
        sem_post(&sem_B1);
        
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
        sem_post(&sem_A1);
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

int main() {
    pthread_t threads[THREAD_NUM];

    sem_init(&sem_A1, 0, 1);
    sem_init(&sem_B1, 0, 0);
    sem_init(&sem_C, 0, 0);
    sem_init(&sem_A2, 0, 0);
    sem_init(&sem_B2, 0, 0);

    pthread_create(&threads[0], NULL, &process_A, NULL);
    pthread_create(&threads[1], NULL, &process_B, NULL);
    pthread_create(&threads[2], NULL, &process_C, NULL);

    for (int i=0; i<THREAD_NUM; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Fallo al hacer el join de los threads.\n");
            fflush(stdout);
        }
    }

    sem_destroy(&sem_A1);
    sem_destroy(&sem_B1);
    sem_destroy(&sem_C);
    sem_destroy(&sem_A2);
    sem_destroy(&sem_B2);

    printf("\n");

    return 0;
}