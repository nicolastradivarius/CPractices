#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

// objetivo: imprimir numeros del 1 al 10.
// los primeros 5 los imprime el hijo, y los otros 5 el padre.

int main() {
    pid_t pid = fork();
    int n;

    if (pid == 0) {
        n = 1;
        printf("Soy el hijo, voy a imprimir los numeros del 1 al 5\n");
        for (int i = 0; i < 5; i++) {
            printf("%d\n", n);
            n++;
        }
        fflush(stdout);
    }
    else if (pid > 0) {
        waitpid(pid, NULL, 0);

        n = 6;
        printf("Soy el padre, voy a imprimir los numeros del 6 al 10\n");
        for (int i = 0; i < 5; i++) {
            printf("%d\n", n);
            n++;
        }
    }
}