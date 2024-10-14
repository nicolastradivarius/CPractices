#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 3
#define MAX_ELEMENTS 6

int A[MAX_ELEMENTS];
int B[MAX_ELEMENTS];
FILE* main_file;
char main_filename[20];


void* tarea_suma(void* arg) {
    int i = *(int*)arg;
    int sum1 = A[i*2] + B[i*2];
    int sum2 = A[i*2+1] + B[i*2+1];

    char filename[20];
    sprintf(filename, "result_%d.txt", i);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        pthread_exit(NULL);
    }
    fprintf(file, "%d %d", sum1, sum2);
    fclose(file);

    pthread_exit(NULL);
}

void* tarea_producto(void* arg) {
    int i = *(int*)arg;
    int prod1 = A[i*2] * B[i*2];
    int prod2 = A[i*2+1] * B[i*2+1];

    char filename[20];
    sprintf(filename, "result_%d.txt", i);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        pthread_exit(NULL);
    }
    fprintf(file, "%d %d", prod1, prod2);
    fclose(file);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    // guardamos los argumentos para cada thread ya que la idea es pasarle una variable interna a un loop
    int thread_args[NUM_THREADS];

    srand(time(NULL));
    for (int i = 0; i < MAX_ELEMENTS; i++) {
        A[i] = rand() % 11; // Valores entre 0 y 10
        B[i] = rand() % 11; // Valores entre 0 y 10
    }

    // Tarea 1: proceso principal asigna valores a los arreglos A y B
    printf("A[] = ");
    for (int i = 0; i < MAX_ELEMENTS; i++) {
        printf("%d ", A[i]);
    }
    printf("\n");
    printf("B[] = ");
    for (int i = 0; i < MAX_ELEMENTS; i++) {
        printf("%d ", B[i]);
    }
    printf("\n");

    // Tarea 2: Crear y ejecutar hilos para la tarea de suma
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, tarea_suma, (void*)&thread_args[i]);
    }

    // Esperar a que los hilos de suma terminen
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Tarea 3: Mostrar resultados finales de las sumas
    printf("S[] = ");
    for (int i = 0; i < NUM_THREADS; i++) {
        char str[6];
        sprintf(main_filename, "result_%d.txt", i);

        main_file = fopen(main_filename, "r");
        if (main_file == NULL) {
            perror("Error al abrir el archivo");
            exit(1);
        }
        if (fgets(str, 6, main_file) != NULL) {
            printf("%s ", str);
        }
        fclose(main_file);
    }
    printf("\n");

    // Tarea 4: Crear y ejecutar hilos para la tarea de multiplicación
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, tarea_producto, (void*)&thread_args[i]);
    }

    // Esperar a que los hilos de multiplicación terminen
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Tarea 5: Mostrar resultados finales de las multiplicaciones
    printf("M[] = ");
    for (int i = 0; i < NUM_THREADS; i++) {
        char str[6];
        sprintf(main_filename, "result_%d.txt", i);

        main_file = fopen(main_filename, "r");
        if (main_file == NULL) {
            perror("Error al abrir el archivo");
            exit(1);
        }
        if (fgets(str, 6, main_file) != NULL) {
            printf("%s ", str);
        }
        fclose(main_file);
    }
    printf("\n");

    return 0;
}