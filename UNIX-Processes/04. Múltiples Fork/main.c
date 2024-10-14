#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main() {

    pid_t pid1 = fork();
    pid_t pid2 = fork();

    // el esquema de procesos es el siguiente:
    // padre
    //  |
    // fork() ------> hijo 1
    //  |               |
    // fork() ------- fork() ------> hijo 2 y nieto 1
    //  |               |               |       |

    if (pid1 == 0) {
        if (pid2 == 0) {
            printf("Soy el proceso nieto 1, y mi pid es %d\n", getpid());
        }
        else {
            printf("Soy el proceso hijo 1, mi pid es %d\n", getpid());
            wait(NULL);
        }
    }
    else if (pid2 == 0) {
        printf("Soy el proceso hijo 2, mi pid es %d\n", getpid());
    }
    else {
        waitpid(pid1, NULL, 0);
        printf("Terminé de esperar por el hijo 1\n");
        waitpid(pid2, NULL, 0);
        printf("Terminé de esperar por el hijo 2\n");
    }
    
    return 0;
}