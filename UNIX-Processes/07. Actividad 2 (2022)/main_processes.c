#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define NUM_PROCESSES 3
#define MAX_ELEMENTS 6

// Tenemos 7 procesos, siendo P1 el principal.
// P1 primero crea tres hijos, luego espera por ellos y hace algo de procesamiento.
// Luego crea otros tres hijos y vuelve a esperar por ellos para hacer procesamiento.


int main() {
    int A[MAX_ELEMENTS];
    int B[MAX_ELEMENTS];

    // descriptor de archivo que solo va a usar el proceso principal (no se forkea nunca)
    FILE* main_file;
    char main_filename[20];

    srand(time(NULL));

    // Tarea 1: proceso principal asigna valores a los arreglos A y B (y de paso los imprimimos)
    for (int i=0; i<MAX_ELEMENTS; i++) {
        A[i] = rand() % (10 + 1 - 1) + 0;
        B[i] = rand() % (10 + 1 - 1) + 0;
    }
    printf("A[] = ");
    for (int i=0; i<MAX_ELEMENTS; i++) {
        printf("%d ", A[i]);
    }
    printf("\nB[] = ");
    for (int i=0; i<MAX_ELEMENTS; i++) {
        printf("%d ", B[i]);
    }
    printf("\n");
    fflush(stdout);

    // ---------------------------------------------------

    // Proceso principal crea tres hijos para la tarea 2.
    for (int i=0; i<NUM_PROCESSES; i++) {
        pid_t pid = fork();

        // Tarea 2: cada proceso hijo obtiene la suma de dos elementos correspondientes
        // por ejemplo, P2 puede sumar A[i] + B[i] y también A[i+3] + B[i+3]
        // El resultado se guarda en un archivo.
        if (pid == 0) {
            int sum1 = A[i*2] + B[i*2];
            int sum2 = A[i*2+1] + B[i*2+1];

            // descriptor de archivo vinculado a cada proceso.
            FILE *file;
            char filename[20];

            sprintf(filename, "result_%d.txt", i);

            // Método para vaciar el archivo en caso de que tenga contenido.
            file = fopen(filename, "w");
            fclose(file);

            // Cada proceso abre un archivo distinto con el modo "append" para agregar cosas al final del archivo.
            file = fopen(filename, "a");
            if (file == NULL) {
                perror("Error al abrir el archivo");
                exit(-1);
            }

            // Escribir resultados
            fprintf(file, "%d %d", sum1, sum2);
            fclose(file);

            exit(0);
        } else if (pid < 0) {
            // Error al crear el proceso hijo
            perror("fork failed");
            exit(1);
        }
    }

    // Proceso principal espera por sus procesos hijos para la tarea 2.
    for (int i=0; i<NUM_PROCESSES; i++) {
        wait(NULL);
    }

    // ---------------------------------------------------

    // Tarea 3: el proceso principal muestra por pantalla la suma de los vectores.
    printf("S[] = ");
    for (int i=0; i<NUM_PROCESSES; i++) {
        char str[6];

        sprintf(main_filename, "result_%d.txt", i);

        main_file = fopen(main_filename, "r");
        if (main_file == NULL) {
            perror("Error al abrir el archivo");
            exit(-1);
        }

        if (fgets(str, 6, main_file) != NULL) {
            printf("%s ", str);
            fflush(stdout);
        }

        fclose(main_file);
    }
    printf("\n");
    fflush(stdout);
    // ---------------------------------------------------

    // El proceso principal vuelve a crear tres hijos para la Tarea 4
    for (int i=0; i<NUM_PROCESSES; i++) {
        pid_t pid = fork();

        // Tarea 4: análoga a la tarea 2 pero con multiplicación. Reutilizamos los mismos archivos vacíandolos primero.
        if (pid == 0) {
            FILE* file;
            char filename[20];
            sprintf(filename, "result_%d.txt", i);

            int prod1 = A[i*2] * B[i*2];
            int prod2 = A[i*2+1] * B[i*2+1];

            // cada proceso vacía el archivo.
            file = fopen(filename, "w");
            fclose(file);

            // Cada proceso abre el archivo con el modo "append" para agregar cosas al final del archivo.
            file = fopen(filename, "a");
            if (file == NULL) {
                perror("Error al abrir el archivo");
                exit(-1);
            }

            // Escribir resultados
            fprintf(file, "%d %d", prod1, prod2);
            fclose(file);

            exit(0);
        }
    }

    // Proceso principal espera por sus procesos hijos para la tarea 4.
    for (int i=0; i<NUM_PROCESSES; i++) {
        wait(NULL);
    }

    // ---------------------------------------------------

    // Tarea 5: el proceso principal muestra por pantalla la finalización de las actividades.
    printf("M[] = ");
    for (int i=0; i<NUM_PROCESSES; i++) {
        char str[6];

        sprintf(main_filename, "result_%d.txt", i);

        main_file = fopen(main_filename, "r");
        if (main_file == NULL) {
            perror("Error al abrir el archivo");
            exit(-1);
        }

        if (fgets(str, 6, main_file) != NULL) {
            printf("%s ", str);
        }

        fclose(main_file);
    }
    printf("\nFinalizaron las actividades.\n");
    fflush(stdout);

    return 0;
}