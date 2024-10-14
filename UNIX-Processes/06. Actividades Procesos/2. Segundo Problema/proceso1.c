#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // el file descriptor es pasado como argumento
    FILE *file = fdopen((int)atoi(argv[1]), "a");

    for (int i = 0; i < 100000; i++) {
        int a = 1, b = 2, c = 3, d = 4;
        fprintf(file, "Operaciones aritméticas\n");
    }
    exit(0);
}