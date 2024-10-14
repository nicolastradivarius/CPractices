#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void main() {

    for (int i = 1; i <= 10000; i++) {
        printf("%d", i);
    }
}