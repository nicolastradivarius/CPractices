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

#define PASSENGERS 8

/**
 * Suponga que hay n pasajeros y un auto. Los pasajeros esperan para tomar viajes en el auto, el cual puede tener hasta 4 ocupantes. Sin embargo, el auto sólo puede andar cuando está lleno.
 * 
 * (a) Considere un modelo que contemple las distintas acciones del problema.
 * (b) Resuelva el problema para las acciones del pasajero y el auto. Use semáforos para la sincronización e hilos para modelar los pasajeros y el auto.
 * 
 * Respuesta (a): podemos tener un semáforo que controle la cantidad de lugares disponibles que tiene el auto. Cada pasajero que llega al auto, intenta tomar un lugar. Si el auto está lleno, entonces los pasajeros se quedarán esperando a que regrese del viaje. Luego del viaje, todos los pasajeros que estaban en el auto se van, con lo cual el semáforo vuelve a su valor inicial (4).
 * Necesito asegurar que una vez empezado el viaje, los pasajeros no se salgan del auto (en términos de código, que no posteen el semáforo antes de que termine el viaje)
 * 
 * Tenemos cuatro semáforos, uno para la cantidad de asientos disponibles (inicializado en 4), otro para indicar cuándo puede salir del auto un pasajero, otro para indicar cuándo pueden entrar otros cuatro pasajeros, y otro para el viaje (para saber cuándo el auto puede iniciar el viaje, inicializado en 0).
 * También un mutex para que dos pasajeros no entren o salgan al mismo tiempo.
 * 
 * Modelo para el pasajero:
 *  ¿Cuál es el protocolo para entrar al coche?
 * lock(mutex) para que no entremos al mismo tiempo que otro pasajero
 * wait(sem_asientos_libres) para chequear si hay lugar en el auto
 * wait(sem_pueden_entrar_cuatro) para chequear si el auto ya está completamente libre.
 * try_wait(sem_asientos_libres) para intentar tomar un lugar en el auto
 *  si no pude decrementar, entonces soy el último pasajero en entrar, hago signal(sem_viaje) para indicar que el auto inicie el viaje
 *  si pude decrementar, es porque aún hay lugar en el auto, hago signal(sem_asientos_libres) para recuperar el decremento.
 * unlock(mutex) 
 * 
 * ¿Cuál es el protocolo para salir del coche? El pasajero sólo puede salir del coche si el auto no está en viaje.
 * El pasajero simplemente hace sem_wait(sem_salida) para salir del auto. El control de cuándo lo hace lo tiene el auto.
 * 
 * Modelo para el coche:
 * ¿Cuál es el protocolo para iniciar el viaje?
 * Se queda esperando a que el ultimo pasajero le indique que puede iniciar el viaje: sem_wait(sem_viaje)
 * Luego de terminar el viaje, permite a los pasajeros salir, libera los asientos y permite que los pasajeros entren.
 * La idea es que los pasajeros entren de a cuatro, y que sean distintos (porque los otros ya se fueron).
 * 
 * 
 */

sem_t sem_asientos_libres, sem_viaje; // controlan cuántos asientos libres hay y si se puede iniciar o no el viaje.
sem_t sem_salida; // se usa para indicar que los pasajeros pueden salir del auto
sem_t sem_pueden_entrar_cuatro; // se usa en conjunto con sem_asientos_libres para controlar que los pasajeros suban al auto de a grupos
pthread_mutex_t mutex_in_out; // controla que dos pasajeros no entren o salgan al mismo tiempo

void *pasajero(void *args) {
    int num_pasaj = (int)(size_t)args;

    // protocolo de entrada al auto
    pthread_mutex_lock(&mutex_in_out);
    sem_wait(&sem_asientos_libres);
    sem_wait(&sem_pueden_entrar_cuatro);
    printf("Pasajero %d subiendo al auto. ", num_pasaj);
    fflush(stdout);
    if (sem_trywait(&sem_asientos_libres) < 0) {
        printf("Soy el último pasajero en entrar.");
        fflush(stdout);
        sem_post(&sem_viaje);
    } else {
        sem_post(&sem_asientos_libres);
    }
    printf("\n");
    fflush(stdout);
    pthread_mutex_unlock(&mutex_in_out);

    // protocolo de salida del auto
    sem_wait(&sem_salida);
    printf("Pasajero %d saliendo del auto.\n", num_pasaj);
    fflush(stdout);
    
    pthread_exit(NULL);
}

void *coche(void *args) {
    for (int i = 0; i < 2; i++) {
        sem_wait(&sem_viaje);
        printf("\n");
        printf("Auto empezó el viaje!\n");
        fflush(stdout);

        printf("Auto terminó el viaje!\n");
        printf("\n");
        fflush(stdout);

        for (int i = 0; i < 4; i++) {
            sem_post(&sem_salida);
            sem_post(&sem_asientos_libres);
        }
        for (int j = 0; j < 4; j++) {
            sem_post(&sem_pueden_entrar_cuatro);
        }
    }
    printf("El auto desaparece!\n");
    fflush(stdout);
    pthread_exit(NULL);
}

int main() {
    // inicializar semáforo
    sem_init(&sem_asientos_libres, 0, 4);
    sem_init(&sem_viaje, 0, 0);
    sem_init(&sem_salida, 0, 0);
    sem_init(&sem_pueden_entrar_cuatro, 0, 4);

    // inicializar mutex
    pthread_mutex_init(&mutex_in_out, NULL);

    // crear hilos
    pthread_t pasajeros[PASSENGERS];
    pthread_t vehiculo;

    pthread_create(&vehiculo, NULL, &coche, NULL);
    for (int i=0; i<PASSENGERS; i++) {
        pthread_create(&pasajeros[i], NULL, &pasajero, (void *)(size_t)i);
    }

    // esperar a que terminen los hilos
    for (int i=0; i<PASSENGERS; i++) {
        pthread_join(pasajeros[i], NULL);
    }
    pthread_join(vehiculo, NULL);

    // destruir semáforo
    sem_destroy(&sem_asientos_libres);
    sem_destroy(&sem_viaje);
    sem_destroy(&sem_salida);

    pthread_mutex_destroy(&mutex_in_out);

    return 0;

}