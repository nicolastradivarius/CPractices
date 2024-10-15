/*En una competencia de atletismo hay tres tipos de participantes.
Lanzadores de jabalina, lanzadores de martillo y corredores.
A primera hora del dia se da acceso libre para que puedan entrenar.
Por seguridad se establecen restricciones para el uso de las instalaciones:

    - Un atleta lanzador de martillo o jabalina debe esperar para entrar a las instalaciones a que no haya ningun atleta,
    sea del tipo que sea.

    - Si hay un atleta lanzador de jabalina o martillo utilizando las instalaciones, un nuevo atleta que llegue,
    sea del tipo que sea, debe esperar para entrar a que el anterior termine    

    - Si en las instalaciones hay algun atleta corredor y llega un nuevo atleta corredor, este entrará en las
    instalaciones incluso aunque hubiese atletas lanzadores esperando para entrar.

a) Implementar el modelo empleando semaforos e hilos
b) Describa la estrategia utilizada
*/

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
#include <stdbool.h>

sem_t sem_instalaciones, sem_corredores;
pthread_mutex_t mutex_corredores;

void *lanzadorJabalina(void *arg)
{
    sem_wait(&sem_instalaciones);

    printf("Lanzador jabalina entrenando\n");
    fflush(stdout);
    sleep(rand() % 3); // Simula el tiempo de entrenamiento

    printf("Lanzador terminó de entrenar\n\n");
    fflush(stdout);

    sem_post(&sem_instalaciones);

    return NULL;
}

void *lanzadorMartillo(void *arg)
{
    sem_wait(&sem_instalaciones);

    printf("Lanzador martillo entrenando\n");
    fflush(stdout);
    sleep(rand() % 3); // Simula el tiempo de entrenamiento

    printf("Lanzador terminó de entrenar\n\n");
    fflush(stdout);

    sem_post(&sem_instalaciones);

    return NULL;
}

void *corredor(void *arg)
{
    pthread_mutex_lock(&mutex_corredores);

    int ret = sem_trywait(&sem_corredores);
    if (ret == -1) { // si no fue posible decrementar (soy el primer corredor)
        sem_wait(&sem_instalaciones);
        printf("Soy el primer corredor. ");
    } else {
        sem_post(&sem_corredores);
    }
    sem_post(&sem_corredores);

    printf("Corredor entrenando\n");
    fflush(stdout);
    
    pthread_mutex_unlock(&mutex_corredores);
    
    sleep(rand() % 4); // Simula el tiempo de entrenamiento
    printf("Corredor terminó de entrenar\n\n");
    fflush(stdout);

    pthread_mutex_lock(&mutex_corredores);
    sem_wait(&sem_corredores);
    int corredores_rest;
    sem_getvalue(&sem_corredores, &corredores_rest);
    printf("Corredores restantes: %d\n\n", corredores_rest);
    fflush(stdout);

    ret = sem_trywait(&sem_corredores);
    if (ret == -1) { //no fue posible decrementar, soy el último
        sem_post(&sem_instalaciones);
    } else {
        // sumo lo que desconté
        sem_post(&sem_corredores);
    }

    pthread_mutex_unlock(&mutex_corredores);

    return NULL;
}

int main()
{
    srand(getpid());

    pthread_t atletas[15];
    sem_init(&sem_instalaciones, 0, 1);
    sem_init(&sem_corredores, 0, 0);
    pthread_mutex_init(&mutex_corredores, NULL);

    // Crear hilos para lanzadores y corredores
    
    for (int i = 0; i < 15; i = i+3)
    {
        pthread_create(&atletas[i], NULL, lanzadorJabalina, NULL);
        pthread_create(&atletas[i+1], NULL, lanzadorMartillo, NULL);
        pthread_create(&atletas[i+2], NULL, corredor, NULL);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < 15; i++)
    {
        pthread_join(atletas[i], NULL);
    }

    sem_destroy(&sem_instalaciones);
    pthread_mutex_destroy(&mutex_corredores);

    return 0;
}