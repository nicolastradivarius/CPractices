#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Código para el atleta lanzador de jabalinas
 */

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

    msgrcv(queue, &msg, msg_length, 1, 0);
    printf("Lanzador de jabalina entrenando\n");
    fflush(stdout);

    sleep(rand() % 2); // Simula el tiempo de entrenamiento

    printf("Lanzador de jabalina terminó de entrenar\n\n");
    fflush(stdout);

    msg.type = 1;
    msg.confirmation = 'J';
    msgsnd(queue, &msg, msg_length, 0);

    return 0;
}