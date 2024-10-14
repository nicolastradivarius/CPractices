#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

// Imprimir la secuencia ABABABAC_ABABABAC utilizando colas de mensajes
// Nota: msgrcv y msgsnd tienen el parametro msgflg que puede ser IPC_NOWAIT o 0
// Si es cero, entonces los hilos que llamen a estas funciones deben bloquearse hasta que:
// - msgrcv: haya un mensaje en la cola que cumpla con el tipo especificado

#define KEY 1234
#define ITERATIONS 100

/*
El tipo 1 corresponde a la letra A
El tipo 2 corresponde a la letra B
El tipo 3 corresponde a la letra C
*/

struct msg_buffer {
    long type;
    char confirmation;
};

int msg_length = sizeof(struct msg_buffer) - sizeof(long);

void process_A() {
    int queue = msgget(KEY, 0666);
    struct msg_buffer msg;
    int i = 0;

    while (i < ITERATIONS) {
        for (int j = 0; j < 4; j++) {
            msgrcv(queue, &msg, msg_length, 1, 0);
            printf("A");
            fflush(stdout);
            msg.type = 2;
            msg.confirmation = '1';
            if (j != 3) {
                msgsnd(queue, &msg, msg_length, 0);
            }
        }
        msg.type = 3;
        msgsnd(queue, &msg, msg_length, 0);
        i++;
    }
}

void process_B() {
    int queue = msgget(KEY, 0666);
    struct msg_buffer msg;
    int i = 0;

    while (i < ITERATIONS) {
        for (int j = 0; j < 3; j++) {
            msgrcv(queue, &msg, msg_length, 2, 0);
            printf("B");
            fflush(stdout);
            msg.type = 1;
            msg.confirmation = '1';
            msgsnd(queue, &msg, msg_length, 0);
        }
        i++;
    }
}

void process_C() {
    int queue = msgget(KEY, 0666);
    struct msg_buffer msg;
    int i = 0;

    while(i < ITERATIONS) {
        msgrcv(queue, &msg, msg_length, 3, 0);
        printf("C");
        fflush(stdout);
        msg.type = 1;
        msg.confirmation = '1';
        msgsnd(queue, &msg, msg_length, 0);
        i++;
    }
}

int main() {
    struct msg_buffer msg;
    msg.type = 1;
    msg.confirmation = '1';
    int queue = msgget(KEY, 0666 | IPC_CREAT); // cola entre A y B: escribe A y lee B

    // envía un mensaje a la queue para iniciar la secuencia
    msgsnd(queue, &msg, msg_length, 0);

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            switch (i) {
                case 0:
                    process_A();
                    break;
                case 1:
                    process_B();
                    break;
                case 2:
                    process_C();
                    break;
            }
            exit(0);
        }
    }

    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    printf("\n");
    fflush(stdout);

    msgctl(queue, IPC_RMID, NULL);

    return 0;
}