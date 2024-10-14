#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/file.h>

#define NUM_PROCESSES 3

// En esta versión del problema, cada proceso hijo cuenta las líneas correspondientes al archivo que le corresponde,
// y cada uno de ellos escribe en el archivo resultados.txt. Para eso, se necesitó lockear el archivo para evitar que
// varios procesos estén escribiendo a la vez en el archivo, provocando que, si ya hay uno escribiendo, el resto deba esperar.
// Sin embargo, no se tiene en cuenta las condiciones de carrera, por lo cual un proceso, llamemoslo P2 (que lee el texto2.txt),
// podría lockear primero y por lo tanto escribir primero en el archivo, haciendo que el proceso P1 escriba después.
// Si la idea es leer los resultados en orden, no se estaría consiguiendo debido a dichas condiciones de carrera.

void random_text(char* str, int num) {
    str[0] = rand() % ('z' - 'a' + 1) + 'a';
    for (int i=1; i<num-1; i++) {
        if ((rand() % 10 % num == 0) && str[i-1] != ' ' && str[i+1] != ' ') {
            str[i] = ' ';
        } else if ((rand() % 12 % num == 0) && str[i-1] != '\n' && str[i+1] != '\n') {
            str[i] = '\n';
        } else {
            str[i] = rand() % ('z' - 'a' + 1) + 'a';
        }
    }
    str[num] = '\0';
}

int count_lines(FILE* file_read) {
    char *buffer = NULL;
    size_t bufsiz = 0;
    ssize_t nbytes;
    int cant_lineas = 0;
    if (file_read == NULL) {
        perror("Error al abrir el archivo");
        exit(-1);
    }

     while ((nbytes = getline(&buffer, &bufsiz, file_read)) != -1) {
        cant_lineas++;
    }
    free(buffer);

    return cant_lineas;
}

int main() {
    srand(time(NULL));
    char str[256];
    struct flock lock;
    
    // Tarea 1: el proceso principal crea tres archivos de texto con un párrafo de varias lineas
    for (int i = 0; i < NUM_PROCESSES; i++) {
        FILE *file;
        char filename[20];

        sprintf(filename, "texto%d.txt", i);

        // Método para vaciar el archivo en caso de que tenga contenido.
        file = fopen(filename, "w");
        fclose(file);

        // Cada proceso abre un archivo distinto con el modo "append" para agregar cosas al final del archivo.
        file = fopen(filename, "a");
        if (file == NULL) {
            perror("Error al abrir el archivo");
            exit(-1);
        }

        random_text(str, 255);
        fprintf(file, "%s", str);
        fflush(stdout);

        fclose(file);
    }

    // ------------------------------------------------------------------------------------------------------

    // Tarea 2: cada proceso hijo debe contar la cantidad de líneas de cada archivo.

    // vacíamos el archivo si tiene algo
    FILE* fd = fopen("resultados.txt", "w");
    fclose(fd);
    int file_write;

    for (int i=0; i < NUM_PROCESSES; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            FILE *file_read;
            char filename[20];
            int cant_lineas = 0;

            sprintf(filename, "texto%d.txt", i);

            file_read = fopen(filename, "r");

            cant_lineas = count_lines(file_read);
           
            fclose(file_read);

            // abrimos el archivo resultados.txt con una función diferente
            file_write = open("resultados.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
            if (file_write == -1) {
                perror("Error al abrir el archivo resultados.txt");
                exit(-1);
            }

            // seteamos las flags para el lock.
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;

            // lockeamos el archivo
            if (fcntl(file_write, F_SETLKW, &lock) == -1) {
                perror("Error al bloquear el archivo");
                close(file_write);
                exit(-1);
            }

            // escribimos la cantidad de lineas en el archivo.
            dprintf(file_write, "Cantidad de líneas del archivo %s: %d\n", filename, cant_lineas);
            if (i==0) {
                sleep(2);
            }

            // quitamos el lockeo del archivo
            lock.l_type = F_UNLCK;

            if (fcntl(file_write, F_SETLK, &lock) == -1) {
                perror("Error al desbloquear el archivo");
                close(file_write);
                exit(-1);
            }

            close(file_write);
            exit(0);
        } else if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
    }

    for (int i=0; i<NUM_PROCESSES; i++) {
        wait(NULL);
    }

    // ------------------------------------------------------------------------------------------------------

    // Tarea 3: el proceso principal lee el archivo resultados.txt y muestra los resultados por pantalla.
    file_write = open("resultados.txt", O_RDONLY, 0644);
    if (file_write == -1) {
        perror("Error al abrir el archivo resultados.txt");
        exit(-1);
    }

    char buffer[1024];
    while (read(file_write, buffer, sizeof(buffer)) != 0) {
        printf("%s\n", buffer);
    }

    close(file_write);

    return 0;
}