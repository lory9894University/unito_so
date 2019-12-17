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

extern int shmId, flagShm;/*processes already have in their stack the id of the shared mem*/
extern int pawnMoveSem, msgPawn;
table *sharedTable;
flag *flags;
pawnDirection directives;


void pawnHandler() {
    int i;
#ifdef DEBUG
    printf("%d pawn: game finished, exiting\n", getpid());
#endif
    for (i = 0; i < sharedTable->height; ++i) {
        shmdt(sharedTable->matrix[i]);
    }
    shmdt(sharedTable->semMatrix);
    shmdt(sharedTable->matrix);
    shmdt(sharedTable);
    exit(0);
}


void tookMyFlag() {
    int reachable;
    reachable = (directives.newDirectives.objective2X + directives.newDirectives.objective2Y) <=
                directives.newDirectives.movesLeft;
    if (directives.newDirectives.objectiveId != directives.newDirectives.objective2Id && reachable) {
        directives.newDirectives.objectiveId = directives.newDirectives.objective2Id;
        directives.newDirectives.objectiveX = directives.newDirectives.objective2X;
        directives.newDirectives.objectiveY = directives.newDirectives.objective2Y;
    } else {
        directives.newDirectives.objectiveId = -1;
        directives.newDirectives.objectiveX = -1;
        directives.newDirectives.objectiveY = -1;
    }
}

void roundEnded() {
    directives.newDirectives.movesLeft = 0;
}

void createPawn(int posX, int posY) {
    struct sigaction sa, se;

    /*sigaction setting*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler = pawnHandler;
    sigaction(SIGUSR1, &sa, NULL);

    bzero(&sa, sizeof(se));
    se.sa_handler = roundEnded;
    sigaction(SIGUSR2, &se, NULL);


#ifdef DEBUG
    fprintf(stderr, "pid: %d , i'm a pawn. created by %d\n", getpid(), getppid());
    //sleep(10); /*thanks debugger*/
#endif

    sharedTable = shmat(shmId, NULL, 0);
    TEST_ERROR;
    flags = shmat(flagShm, NULL, 0);
    sharedTable->matrix[posY][posX] = 'T';
}

void moving() {
    /*attendi sulla coda flagQueue con IPC_NOWAIT e MSG_COPY e msgtype = directives.new.id, nel mentre esegui i tuoi spostamenti*/
    while (directives.newDirectives.movesLeft > 0) {

        directives.newDirectives.movesLeft--;
        /*quando prendi una flag invia un messaggio specificando quale bandiera è stata presa sulla coda "flagQueque" con msgtype = flag.id
         * ATTENZIONE ricordati di impostare il campo playerPid nel messaggio, al master serve*/
    }
}

void pawnLife() {

    while (1) {
        /*wait for instructions from player*/
        msgrcv(msgPawn, &directives, sizeof(pawn), getpid(), 0);
        semHandling(pawnMoveSem, 0, 0); /*wait for master to start the round*/
#ifdef DEBUG
        fprintf(stderr, "%d directives. posx: %d posy %d, dirx: %d diry: %d\n", getpid(),
                directives.newDirectives.positionX, directives.newDirectives.positionY,
                directives.newDirectives.objectiveX, directives.newDirectives.objectiveY);
#endif
        /*quando hai finito le mosse, oppure se ricevi il segnale di fine turno invia la tua posizione e le mosse residue al player*/
        moving();
        msgsnd(msgPawn, &directives, sizeof(pawn), getpid());
    }
}
