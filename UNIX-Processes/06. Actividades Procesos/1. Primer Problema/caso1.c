#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


/*
Un proceso padre debe crear dos procesos hijos, uno de ellos ejecutar
el comando de listar un directorio (usar el /etc), y el otro mostrar en
pantalla un contador (de 1 a 10000). El padre deberá esperar la
finalización de ambos procesos, y cuando esto ocurra lo indicará con un
mensaje en pantalla.
Primer caso: en el proceso padre se encuentra toda la codificación.
*/


void main() {

    int pid1, pid2;

    pid1 = fork();

    if (pid1 == 0) {
        int ret = execl("/bin/ls", "ls", "/etc", NULL);
        if (ret == -1) {
            perror("Error en execl");
            exit(1);
        }
    }

    pid2 = fork();

    if (pid2 == 0) {
        for (int i = 1; i <= 10000; i++) {
            printf("%d", i);
        }
        exit(0);        
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("\nHan terminado mis dos procesos hijos.\n");
}