#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

// la idea es jugar un poco con los wait() y getpid()

int main() {
    pid_t pid = fork();

    printf("Mi PID es %d y el PID de mi padre es %d\n", getpid(), getppid());

    if (pid == 0) {
        // haciendo un sleep en el hijo así nomas, logramos que
        // el padre termine antes que el hijo, y por ende, el
        // hijo sea adoptado por init o systemd (queda zombie).
        sleep(1);
    }
    else if (pid > 0){
        int result = wait(NULL);
        if (result == -1) {
            printf("No hay hijos para esperar\n");
        }
        else {
            printf("Mi hijo con PID %d terminó\n", result);
        }
    }

    return 0;

    

}