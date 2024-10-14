#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

// se crean 4 procesos, primero 1 padre y 1 hijo, luego éste hijo crea
// dos procesos más, siendo padre de ellos. Por eso tenemos
// la palabra padre 2 veces, y la palabra hijo 2 veces en la consola.
int main() {
    pid_t pid = fork();
    pid_t pid2 = fork();

    if (pid == 0) {
        printf("Soy el proceso hijo\n");
        
    } else if (pid > 0) {
        printf("Soy el proceso padre\n");
    } else {
        printf("Error al crear el proceso hijo\n");
    }
}