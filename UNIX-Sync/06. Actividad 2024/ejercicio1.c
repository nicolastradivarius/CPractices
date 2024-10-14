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

/*Implementar codigo para la secuencia ABABABACABABABAC
a) Existe un unico hilo por cada letra y estos se sincronizan utilizando semaforos (No
intervienen otros hilos).
b) No utilice variables globales para sincronizar.
c) Busque optimizar el uso de los recursos.
*/

sem_t sem_A, sem_B, sem_C;
pthread_t threads[3];

void *process_A()
{
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            sem_wait(&sem_A);
            printf("A");
            fflush(stdout);
            if (i != 3) {
                sem_post(&sem_B);
            }
        }
        sem_post(&sem_C);
    }
    return NULL;
}

void *process_B()
{
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            sem_wait(&sem_B);
            printf("B");
            fflush(stdout);
            sem_post(&sem_A);
        }
    }
    return NULL;
}

void *process_C()
{
    for (int j = 0; j < 5; j++)
    {
        sem_wait(&sem_C);
        printf("C");
        printf("_");
        fflush(stdout);
        sem_post(&sem_A);
    }
    return NULL;
}

int main()
{
    sem_init(&sem_A, 0, 1);
    sem_init(&sem_B, 0, 0);
    sem_init(&sem_C, 0, 0);

    pthread_create(&threads[0], NULL, process_A, NULL);
    pthread_create(&threads[1], NULL, process_B, NULL);
    pthread_create(&threads[2], NULL, process_C, NULL);

    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("\n");
    fflush(stdout);
    
    sem_destroy(&sem_A);
    sem_destroy(&sem_B);
    sem_destroy(&sem_C);

    return 0;
}