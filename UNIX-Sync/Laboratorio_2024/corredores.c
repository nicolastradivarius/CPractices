#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>

#define KEY 4321
#define ITERATIONS 10

struct msg_buffer {
    long type;
    char confirmation;
};

int msg_length = sizeof(struct msg_buffer) - sizeof(long);

int main(int argc, char *args[]) {
    int queue = msgget(KEY, 0666);
    struct msg_buffer msg;

    // leo de la cola en modo no bloqueante
    int result = msgrcv(queue, &msg, msg_length, 1, IPC_NOWAIT);
    
    while (result == -1) {
        // reintento hasta que se liberen las instalaciones
        result = msgrcv(queue, &msg, msg_length, 1, IPC_NOWAIT);

        // cuando deje de ser -1, es porque soy el primero
        // debo enviar mensajes de tipo 2
    }
    // si ya hay otros corredores, debo leer mensajes de tipo 2 de forma bloqueante para poder entrar
    msgrcv(queue, &msg, msg_length, 2, 0);
    // sin terminar.


}