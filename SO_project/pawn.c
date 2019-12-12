#define _GNU_SOURCE
#include "pawn.h"
#include "shared_res.h"
#include "error_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/msg.h>

#define DEBUG

extern int shmId;/*processes already have in their stack the id of the shared mem*/
extern int pawnMoveSem, msgPawn;
table *sharedTable;

void pawnHandler() {
    int i;
#ifdef DEBUG
    printf("%d pawn: game finished, exiting\n",getpid());
#endif
    for (i = 0; i < sharedTable->height; ++i) {
        shmdt(sharedTable->matrix[i]);
    }
    shmdt(sharedTable->semMatrix);
    shmdt(sharedTable->matrix);
    shmdt(sharedTable);
    exit(0);
}

void createPawn(int posX, int posY) {
    struct sigaction sa;

    /*sigaction setting*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler = pawnHandler;
    sigaction(SIGUSR1, &sa, NULL);

#ifdef DEBUG
    fprintf(stderr, "pid: %d , i'm a pawn. created by %d\n", getpid(), getppid());
    //sleep(10); /*thanks debugger*/
#endif

    sharedTable = shmat(shmId, NULL,
                        0);//TODO: questo costrutto è veramente necessario? il puntatore a sharedTable è già nell'heap
    TEST_ERROR;

    sharedTable->matrix[posY][posX] = 'T';
}

void pawnLife() {
    pawnDirection directives;


    while (1) {
        /*attendi le istruzioni dal player via pipe o msg*/
        msgrcv(msgPawn, &directives, sizeof(pawn), getpid(), 0);
        semHandling(pawnMoveSem, 0, 0); /*wait for master to start the round*/
#ifdef DEBUG
        fprintf(stderr, "%d directives. posx: %d posy %d, dirx: %d diry: %d\n", getpid(),
                directives.newDirectives.positionX, directives.newDirectives.positionY,
                directives.newDirectives.objectiveX, directives.newDirectives.objectiveY);
#endif
        /*muoviti verso la flag primaria, se ricevi il segnale di flag presa calcola la distanza dalla secondaria, se raggiungibile muoviti in direzione della secondaria*/
        /*quando prendi una flag presa invia al master un messaggio specificando quale bandiera è stata presa sulla coda "flagQueque", calcola se puoi raggiungere la falg secondaria e comportati di conseguenza*/
        /*quando hai finito le mosse, oppure se ricevi il segnale di fine turno invia la tua posizione e le mosse residue al player*/
    }
    pawnHandler();
}
