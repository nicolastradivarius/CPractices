#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>


struct info {
    char mensaje[255];
};

#define SEGSIZE sizeof(struct info)
#define KEY ((key_t) (1234))

int main(void) {
    struct info *ctrl; // nuestra estructura para mapear la memoria compartida

    // abrimos para lectura/escritura el objeto de memoria compartida
    int fd = shmget(KEY, SEGSIZE, 0);
    if (fd == -1) {
        perror("Error, el objeto de memoria no se pudo abrir en modo lectura/escritura");
        return 1;
    }

    // mapeamos el objeto de memoria compartida en nuestro espacio de direcciones
    ctrl = (struct info*) shmat(fd, 0, 0);
    if (ctrl == (struct info*) 0) {
        perror("Error, no se pudo mapear el objeto de memoria compartida");
        return 2;
    }

    printf("Mensaje recibido: %s\n", ctrl->mensaje);

    shmdt(ctrl);

    return 0;
}