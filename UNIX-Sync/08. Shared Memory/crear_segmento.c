/**
 * Este programa crea un objeto de memoria "/myMemoryObj"
 * El objeto puede ser encontrado en /dev/shm
 * Para compilar usar el flag "-lrt" (que es la librería necesaria para usar la memoria compartida)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

#define KEY ((key_t) (1234))

struct info {
    char mensaje[255];
};

#define SEGSIZE sizeof(struct info)
#define KEY ((key_t) (1234))

int main(void) {
    struct info *ctrl; // nuestra estructura para mapear la memoria compartida

    // creamos un objeto de memoria compartida
    int fd = shmget(KEY, SEGSIZE, IPC_CREAT | 0666);
    if (fd == -1) {
        perror("Error, no se pudo crear el objeto de memoria compartida");
        return 1;
    }
    
    // mapeamos el objeto de memoria compartida en nuestro espacio de direcciones
    ctrl = (struct info*) shmat(fd, 0, 0);
    if (ctrl == (struct info*) 0) {
        perror("Error, no se pudo mapear el objeto de memoria compartida");
        return 2;
    }

    strcpy(ctrl->mensaje, "Hola, soy el proceso padre");

    shmdt(ctrl);
    //shmctl(fd, IPC_RMID, 0);

    return 0;
}