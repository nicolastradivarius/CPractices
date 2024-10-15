#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * /*En una competencia de atletismo hay tres tipos de participantes.
Lanzadores de jabalina, lanzadores de martillo y corredores.
A primera hora del dia se da acceso libre para que puedan entrenar.
Por seguridad se establecen restricciones para el uso de las instalaciones:

    - Un atleta lanzador de martillo o jabalina debe esperar para entrar a las instalaciones a que no haya ningun atleta,
    sea del tipo que sea.

    - Si hay un atleta lanzador de jabalina o martillo utilizando las instalaciones, un nuevo atleta que llegue,
    sea del tipo que sea, debe esperar para entrar a que el anterior termine

    - Si en las instalaciones hay algun atleta corredor y llega un nuevo atleta corredor, este entrará en las
    instalaciones incluso aunque hubiese atletas lanzadores esperando para entrar.

    Implementarlo con procesos separados y colas de mensajes.
 */

#define KEY 4321

struct msg_buffer {
    long type;
    char confirmation;
};

int msg_length = sizeof(struct msg_buffer) - sizeof(long);

// Proceso padre se encarga de crear los procesos hijos, las colas de mensajes e indicarles el ejecutable que deben correr a sus hijos.

int main () {
    struct msg_buffer msg;
    msg.type = 1;
    msg.confirmation = 'I';

    int queue = msgget(KEY, 0666 | IPC_CREAT);

    msgsnd(queue, &msg, msg_length, 0);

    for (int i = 0; i < 10; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            execl("./lanzadorJabalina", "lanzadorJabalina", NULL);
        }
    }

    for (int i = 0; i < 10; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            execl("./lanzadorMartillo", "lanzadorMartillo", NULL);
        }
    }

    for (int i = 0; i < 10; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            execl("./corredor", "corredor", NULL);
        }
    }

    for (int i = 0; i < 30; i++) {
        wait(NULL);
    }
    
    msgctl(queue, IPC_RMID, NULL);

    return 0;
}