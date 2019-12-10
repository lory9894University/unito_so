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

#define DEBUG

extern int shmId;/*processes already have in their stack the id of the shared mem*/
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

pid_t createPawn(int posX, int posY) {
    pawn thisPawn;
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

    thisPawn.positionX = posX;
    thisPawn.positionY = posY;
    thisPawn.pid = getpid();

    sharedTable->matrix[posY][posX] = 'T';

    return thisPawn.pid;
}

void pawnLife() {

    pawnHandler();
}
