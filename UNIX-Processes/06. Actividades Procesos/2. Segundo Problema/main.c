#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*
Un proceso padre debe crear tres procesos hijos. El padre crea un archivo
denominado salida.txt antes de invocar a los procesos hijos. El proceso padre
debe esperar que los procesos hijos terminen su ejecución y al finalizar debe
agregar en el archivo salida.txt ``Se ha finalizado la actividad". Cada proceso
hijo debe cargar una imagen ejecutable que realice la tarea asignada:
• El primer hijo debe realizar un ciclo de 100000 iteraciones, en cada una de
las iteraciones realiza cuatro operaciones aritméticas y agregar el texto
“Operaciones aritméticas” en el archivo salida.txt.
• El segundo hijo debe mostrar por pantalla el contenido del directorio actual,
esto lo debe realizar 1000 veces, además en cada iteración debe agregar la
siguiente información en el archivo salida.txt ``Estoy mostrando el directorio
actual"
• El tercer hijo debe iterar 20000 veces y en cada una de las iteraciones debe
sumar 1 a una variable entera y lo debe mostrar por pantalla ``El resultado es
...", por último debe agregar esta información en el archivo salida.txt.
*/

void main() {

    int pid1, pid2, pid3;
    FILE *file = fopen("salida.txt", "w");
    
    pid1 = fork();

    if (pid1 == 0) {
        int ret = execl("proceso1", "proceso1", file, NULL);
        if (ret == -1) {
            perror("Error en la llamada al comando ls");
            exit(1);
        }
    }

    pid2 = fork();

    if (pid2 == 0) {
        int ret = execl("proceso2", "proceso2", file, NULL);
        if (ret == -1) {
            perror("Error en la llamada al comando ls");
            exit(1);
        }
    }

    pid3 = fork();

    if (pid3 == 0) {
        int ret = execl("proceso3", "proceso3", file, NULL);
        if (ret == -1) {
            perror("Error en la llamada al comando ls");
            exit(1);
        }
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);

    fprintf(file, "Se ha finalizado la actividad\n");
    fclose(file);

}