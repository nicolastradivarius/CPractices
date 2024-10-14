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

/*
 * Problema del productor-consumidor con buffer limitado:
 *     - Un productor produce elementos y los deposita en un buffer.
 *     - Un consumidor consume elementos del buffer.
 *     - El buffer tiene una capacidad limitada.
 *     - El productor debe esperar si el buffer está lleno.
 *     - El consumidor debe esperar si el buffer está vacío.
 *     - El productor y el consumidor no pueden acceder al buffer al mismo tiempo.
 * 
 * ¿Cuáles son los datos compartidos? el buffer
 * Necesito tres semáforos: uno que me indique cuándo el buffer está vacío (el productor puede producir y el consumidor no), otro que indique cuando está lleno (el productor no puede producir y el consumidor puede consumir) y otro que me indique si el buffer está siendo usado por el productor o el consumidor. Este último puede ser binario o un mutex.
 */

sem_t sem_lleno, sem_vacio, sem_mutex;
int buffer[10];

void *productor(void *arg) {

    do {
        sem_wait(&sem_vacio);
        sem_wait(&sem_mutex);

        // producir


        sem_post(&sem_mutex);
        sem_post(&sem_lleno);
    } while (1);
    
}

void *consumidor(void *arg) {

        do {
            sem_wait(&sem_lleno);
            sem_wait(&sem_mutex);
    
            // consumir
    
            sem_post(&sem_mutex);
            sem_post(&sem_vacio);
        } while (1);
        
}


void main() {

    // inicialización de los semáforos
    sem_init(&sem_lleno, 0, 0);
    sem_init(&sem_vacio, 0, 10);
    sem_init(&sem_mutex, 0, 1);

    pthread_t hilo_productor, hilo_consumidor;

    pthread_create(&hilo_productor, NULL, productor, NULL);
    pthread_create(&hilo_consumidor, NULL, consumidor, NULL);

    pthread_join(hilo_productor, NULL);
    pthread_join(hilo_consumidor, NULL);

    sem_destroy(&sem_lleno);
    sem_destroy(&sem_vacio);
    sem_destroy(&sem_mutex);

    exit(0);
}