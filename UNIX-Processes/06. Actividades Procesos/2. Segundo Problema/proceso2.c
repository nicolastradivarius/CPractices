#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    FILE *file = fdopen((int)atoi(argv[1]), "a");

    for (int i = 1; i <= 1000; i++) {
        // lo que hace system es crear un nuevo proceso hijo que ejecuta el comando que se le pasa como argumento y cuando termina, regresa al proceso padre en el mismo lugar donde se quedó
        int ret = system("ls");
        if (ret == -1) {
            perror("Error en la llamada al comando ls");
            exit(1);
        }
        fprintf(file, "Estoy mostrando el directorio actual\n");
    }

    exit(0);
}