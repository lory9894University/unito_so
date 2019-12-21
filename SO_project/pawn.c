#define _GNU_SOURCE
#include "pawn.h"
#include "shared_res.h"
#include "error_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/msg.h>

#define DEBUG

extern int shmId, flagShm;
extern int pawnMoveSem, msgPawn;
table *sharedTable;
flag *flags;
pawnDirection directives;


void pawnHandler(int signum) {
    int i;
#ifdef DEBUG
    printf("%d pawn: game finished, exiting\n", getpid());
    printf("%d %d\n", directives.newDirectives.positionY, directives.newDirectives.positionX);
#endif
    sharedTable->matrix[directives.newDirectives.positionY][directives.newDirectives.positionX] = 'T';
    //sharedTable->matrix[directives.newDirectives.positionY][directives.newDirectives.positionX]='T';
    for (i = 0; i < sharedTable->height; ++i) {
        shmdt(sharedTable->matrix[i]);
    }
    shmdt(sharedTable->semMatrix);
    shmdt(sharedTable->matrix);
    shmdt(sharedTable);
    shmdt(flags);
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

void roundEnded(int signum) {
    directives.newDirectives.movesLeft = 0;
}

void createPawn(int posX, int posY) {
    struct sigaction sa, se;

    /*sigaction setting*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler = pawnHandler;
    sigaction(SIGUSR1, &sa, NULL);

    bzero(&se, sizeof(se));
    se.sa_handler = roundEnded;
    se.sa_flags = SA_RESTART;
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
    int yVect, xVect;
    /*attendi sulla coda flagQueue con IPC_NOWAIT e MSG_COPY e msgtype = directives.new.id, nel mentre esegui i tuoi spostamenti*/
    /*todo: settare una mask quando la pedina ha cancellato la table e toglierla quando la riscrive*/
    while (directives.newDirectives.movesLeft > 0) {
        yVect = directives.newDirectives.objectiveY - directives.newDirectives.positionY;
        xVect = directives.newDirectives.objectiveX - directives.newDirectives.positionX;
        if (abs(yVect) < abs(xVect)) {
            /*move on X Axis*/
            if (xVect < 0) {
                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                            directives.newDirectives.positionX + 1, RESERVE);
                directives.newDirectives.positionX++;
                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                            directives.newDirectives.positionX, RELEASE);
            } else if (xVect > 0) {
                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                            directives.newDirectives.positionX - 1, RESERVE);
                directives.newDirectives.positionX--;
                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                            directives.newDirectives.positionX, RELEASE);
            } else { /*if the pawn is forced to move on X axis beacause of a deadlock but it's on the right Y axis*/
                directives.newDirectives.movesLeft++;
                //todo: in questo caso gli sto solo dicendo di attendere ancora
            }
        } else {
            /*move on Y Axis*/
            if (yVect < 0) {
                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY + 1],
                            directives.newDirectives.positionX, RESERVE);
                sharedTable->matrix[directives.newDirectives.positionX][directives.newDirectives.positionY] = ' ';
                directives.newDirectives.positionX++;
                sharedTable->matrix[directives.newDirectives.positionX][directives.newDirectives.positionY] = 'T';
                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                            directives.newDirectives.positionX, RELEASE);
            } else if (yVect > 0) {
                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY - 1],
                            directives.newDirectives.positionX, RESERVE);
                sharedTable->matrix[directives.newDirectives.positionX][directives.newDirectives.positionY] = ' ';
                directives.newDirectives.positionX--;
                sharedTable->matrix[directives.newDirectives.positionX][directives.newDirectives.positionY] = 'T';
                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                            directives.newDirectives.positionX, RELEASE);
            } else { /*if the pawn is forced to move on X axis beacause of a deadlock but it's on the right Y axis*/
                directives.newDirectives.movesLeft++;
                //todo: in questo caso gli sto solo dicendo di attendere ancora
            }

        }
        directives.newDirectives.movesLeft--;
        /*quando prendi una flag invia un messaggio specificando quale bandiera è stata presa sulla coda "flagQueque" con msgtype = flag.id
         * ATTENZIONE ricordati di impostare il campo playerPid nel messaggio, al master serve*/
    }
}

void pawnLife() {
    while (1) {
        /*wait for instructions from player*/
        msgrcv(msgPawn, &directives, sizeof(pawn), getpid(), 0);
        while (errno == EINTR) {
            fprintf(stderr, "deep shit pawn\n");
            msgrcv(msgPawn, &directives, sizeof(pawn), getpid(), 0);
        }
        semHandling(pawnMoveSem, 0, 0); /*wait for master to start the round*/
#ifdef DEBUG
        fprintf(stderr, "%d directives. posx: %d posy %d, dirx: %d diry: %d\n", getpid(),
                directives.newDirectives.positionX, directives.newDirectives.positionY,
                directives.newDirectives.objectiveX, directives.newDirectives.objectiveY);
#endif
        /*quando hai finito le mosse, oppure se ricevi il segnale di fine turno invia la tua posizione e le mosse residue al player*/
        sharedTable->matrix[directives.newDirectives.positionY][directives.newDirectives.positionX] = ' ';
        //moving();
        directives.mtype = 1;
        msgsnd(msgPawn, &directives, sizeof(pawn), 0);
        semHandling(pawnMoveSem, 0, RELEASE);
    }
}
