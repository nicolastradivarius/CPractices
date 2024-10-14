#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    FILE *file = fdopen((int)atoi(argv[1]), "a");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    int a = 0;
    for (int i = 1; i <= 20000; i++) {
        a++;
        printf("El resultado es %d\n", a);
        fprintf(file, "El resultado es %d\n", a);
    }

    exit(0);


}