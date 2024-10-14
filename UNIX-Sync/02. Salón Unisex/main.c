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

/** */
Considere un salón unisex donde tanto hombres como mujeres pueden recibir el servicio del salón.
Sin embargo, hay una condición: hombres y mujeres no pueden estar en el salón al mismo tiempo.
La segunda condición es que no puede haber más de tres personas en el salón al mismo tiempo.
Diseñe una solución de sincronización utilizando semáforos (y mutex).

Voy a necesitar tres semáforos: uno para el hombre, otro para la mujer, y otro para el salón.
Los semáforos de hombres y mujeres van a permitir controlar cuántos hombres y mujeres hay en el salón.
Por lo tanto van a estar inicializados en 0. El semáforo del salón en 1.
Voy a necesitar un mutex para proteger la entrada al salón (que no entren dos mujeres a la vez porque
ambas "pensaron" que eran la primera en llegar).
Además, para controlar la cantidad de mujeres u hombres admitidos en el salón, necesito dos semáforos más:
sem_max_mujeres y sem_max_hombres, ambos inicializados en 3.

Imaginemos que una mujer llega al salón.
¿Hay hombres dentro del salón?
Si no hay hombres, entonces ¿hay otras mujeres dentro?
Si no hay mujeres, entonces soy la primer mujer en llegar.

De https://linux.die.net/man/3/sem_trywait vemos que:
    - si el semáforo es cero, entonces no se puede decrementar su valor, pero tampoco bloquearse, entonces retorna -1 y se setea ERRNO en EAGAIN. Nótese que el semáforo sigue siendo cero.
    - si el semáforo es distinto de cero, entonces es posible decrementar, y lo decrementa y retorna 0.
Es importante recalcar que, en caso de que sea posible decrementar, el semáforo efectivamente se decrementa.
Por eso es que vamos a necesitar dos signal, uno para recuperar el decremento, y otro para indicar lo que queríamos indicar; salvo en el protocolo de salida, ya que una vez recuperado el decremento, el semáforo indicará la cantidad de personas que quedaron.

Modelo para la mujer:
    ¿Cuál es el protocolo para entrar al salón? ¿Qué pasa si soy la primera en llegar?
    wait(sem_max_mujeres) para chequear si puedo entrar o no porque esté lleno el salón.
    lock(mutex_mujeres)
    try_wait(sem_mujeres) intento chequear si soy la primera para decidir si bloqueo el salon.
        si retornó cero (pude decrementar), entonces hay otras mujeres dentro, hago signal(sem_mujeres) para incrementar el semaforo (dado que lo desconté en el try_wait).
        si retornó -1 (no pude decrementar), entonces soy la primer mujer, hago wait(sem_salon) para apropiarme del salón.
    signal(sem_mujeres) para sumarme a las mujeres. Cuando salga lo descuento.
    unlock(mutex_mujeres)

    Puedo hacer un sleep para simular un tiempo en el salón.

    ¿Cuál es el protocolo para salir del salón? ¿Qué pasaría si soy la última en salir?
    lock(mutex_mujeres)
    wait(sem_mujeres) para indicar que me fui, me descuento de las mujeres.
    try_wait(sem_mujeres) para ver si soy la última.
        si devuelve cero, entonces todavía hay mujeres dentro, hago signal(sem_mujeres) para devolver el descuento del try_wait.
        si retornó -1, entonces soy la última, hago signal(sem_salon) para liberar el salón (indico que está vacío) y permitir que entren hombres (si los hubiera).
    unlock(mutex_mujeres)
    signal(sem_max_mujeres) para indicar que hay un lugar más libre en el salón.

El modelo para el hombre es análogo al de la mujer, con la diferencia de que se cambian los nombres de los semáforos y mutexes.
*/

sem_t sem_hombres, sem_mujeres, sem_salon, sem_max_hombres, sem_max_mujeres;
pthread_mutex_t mutex_hombres, mutex_mujeres;
int cant_mujeres = 0; 
int cant_hombres = 0;


void *mujer(void *arg) {
    // protocolo de entrada
    sem_wait(&sem_max_mujeres);
    pthread_mutex_lock(&mutex_mujeres);
    int ret = sem_trywait(&sem_mujeres);
    if (ret == -1) { // si no fue posible decrementar (el semaforo es cero)
        sem_wait(&sem_salon);
        printf("Soy la primer mujer en entrar. ");
    } else {
        sem_post(&sem_mujeres);
        printf("Soy mujer y entré al salon. ");
    }
    sem_post(&sem_mujeres); // indica que ya hay mujeres en el salón
    sem_getvalue(&sem_mujeres, &cant_mujeres);
    printf("Somos %d mujeres en el salón.\n", cant_mujeres);
    pthread_mutex_unlock(&mutex_mujeres);

    // protocolo de salida
    pthread_mutex_lock(&mutex_mujeres);
    sem_wait(&sem_mujeres);
    ret = sem_trywait(&sem_mujeres);
    if (ret == -1) {
        printf("Soy la última mujer en salir. ");
        sem_post(&sem_salon); // soy la última mujer, libero el salón
    } else {
        sem_post(&sem_mujeres);
        printf("Soy mujer y me voy del salón. ");
    }
    sem_getvalue(&sem_mujeres, &cant_mujeres);
    printf("Quedan %d mujeres en el salón.\n", cant_mujeres);
    pthread_mutex_unlock(&mutex_mujeres);
    sem_post(&sem_max_mujeres);
    
}

void *hombre(void *arg) {
    // protocolo de entrada
    sem_wait(&sem_max_hombres);
    pthread_mutex_lock(&mutex_hombres);
    int ret = sem_trywait(&sem_hombres);
    if (ret == -1) { // si no fue posible decrementar (el semaforo es cero)
        sem_wait(&sem_salon);
        printf("Soy el primer hombre en entrar. ");
    } else {
        sem_post(&sem_hombres);
        printf("Soy hombre y entré al salón. ");
    }
    sem_post(&sem_hombres); // indica que ya hay hombres en el salón
    sem_getvalue(&sem_hombres, &cant_hombres);
    printf("Somos %d hombres en el salón.\n", cant_hombres);
    pthread_mutex_unlock(&mutex_hombres);

    // protocolo de salida
    pthread_mutex_lock(&mutex_hombres);
    sem_wait(&sem_hombres);
    ret = sem_trywait(&sem_hombres);
    if (ret == -1) {
        printf("Soy el último hombre en salir. ");
        sem_post(&sem_salon); // soy el último hombre, libero el salón
    } else {
        sem_post(&sem_hombres);
        printf("Soy hombre y me voy del salón. ");
    }
    sem_getvalue(&sem_hombres, &cant_hombres);
    printf("Quedan %d hombres en el salón.\n", cant_hombres);
    pthread_mutex_unlock(&mutex_hombres);
    sem_post(&sem_max_hombres);
}

void main() {
    srand(time(NULL));
    // inicializacion de los semaforos y mutex
    sem_init(&sem_hombres, 0, 0);
    sem_init(&sem_mujeres, 0, 0);
    sem_init(&sem_salon, 0, 1);
    sem_init(&sem_max_hombres, 0, 3);
    sem_init(&sem_max_mujeres, 0, 3);

    pthread_mutex_init(&mutex_hombres, NULL);
    pthread_mutex_init(&mutex_mujeres, NULL);

    pthread_t threads[6]; // 3 hilos para hombres y 3 para mujeres

    for (int i=0; i<3; i++) {
        pthread_create(&threads[i+3], NULL, &mujer, NULL);

        pthread_create(&threads[i], NULL, &hombre, NULL);
    }

    for (int i=0; i<6; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem_hombres);
    sem_destroy(&sem_mujeres);
    sem_destroy(&sem_salon);
    sem_destroy(&sem_max_hombres);
    sem_destroy(&sem_max_mujeres);

    pthread_mutex_destroy(&mutex_hombres);
    pthread_mutex_destroy(&mutex_mujeres);

    exit(0);
}

/*
Caso de inanicion:
Soy el primer hombre en entrar. Somos 1 hombres en el salón.
Soy hombre y entré al salón. Somos 2 hombres en el salón.
Soy hombre y entré al salón. Somos 3 hombres en el salón.
Soy hombre y me voy del salón. Quedan 2 hombres en el salón.
Soy hombre y me voy del salón. Quedan 1 hombres en el salón.
Soy el último hombre en salir. Quedan 0 hombres en el salón.
Soy la primer mujer en entrar. Somos 1 mujeres en el salón.
Soy mujer y entré al salon. Somos 2 mujeres en el salón.
Soy mujer y entré al salon. Somos 3 mujeres en el salón.
Soy la última mujer en salir. Quedan 1 mujeres en el salón.
Soy la última mujer en salir. Quedan 0 mujeres en el salón.

*/