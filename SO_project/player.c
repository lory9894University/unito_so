#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "shared_res.h"
#include "player.h"
#include "error_handling.h"
#include "pawn.h"
//#define DEBUG

extern int shmId;/*processes already have in their stack the id of the shared mem*/
extern table *sharedTable;

void playerHandler() {
    int i;
#ifdef DEBUG
    int childPid = 0;
    int status = 0;
    printf("%d player: game finished, waiting child and exiting\n", getpid());
    while ((childPid=wait(&status)) != -1)
        dprintf(2,"PID=%d. Sender (PID=%d) terminated with status %d\n",getpid(),childPid,WEXITSTATUS(status));
#else
    while (wait(NULL) != -1);
#endif
    for (i = 0; i < sharedTable->height; ++i) {
        shmdt(sharedTable->matrix[i]);
    }
    shmdt(sharedTable->semMatrix);
    shmdt(sharedTable->matrix);
    shmdt(sharedTable);
    exit(0);
}

void placePawn(pid_t *pawns) {
    int posX = 0, posY = 0, positionOccupied = 1;

#ifdef DEBUG
    fprintf(stderr, "pid: %d , i'm a pawn\n", getpid());
    sleep(10); /*thanks debugger*/
#endif
    /* casual positioning*/
    /*todo: scrivere nella relazione il fatto che avrei voluto usare un algoritmo di hashing
     * e disporre uniformemente le pedine sulla schacchiera, ma avrebbe senso solo programmando un giocatore singolo.
     * dovendone programmare più essi otterrebbero tutti la stessa posizione e mi ritrovere con pedine affiancate*/
    srand(getpid());
    while (positionOccupied) {
        posX = rand() % sharedTable->base;
        posY = rand() % sharedTable->height;
        if (sharedTable->matrix[posY][posX] == ' ')
            positionOccupied = 0;
    }
    *pawns = createPawn(posX, posY);

}

void playerBirth(int pawnNumber) {
    int i;
    pid_t *pawnArray;
    struct sigaction sa;
    int forkValue = -1;

#ifdef DEBUG
    fprintf(stderr, "pid: %d , i'm a player\n", getpid());
    sleep(10); /*thanks debugger*/
    //printf("%d ", shmId);
#endif
    /*sigaction setting*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler = playerHandler;
    sigaction(SIGUSR1, &sa, NULL);
    sharedTable = shmat(shmId, NULL,
                        0); //TODO: questo costrutto è veramente necessario? il puntatore a sharedTable è già nell'heap
    TEST_ERROR;

    pawnArray = malloc(sizeof(pid_t) * pawnNumber);
    for (i = 0; i < pawnNumber && forkValue != 0; ++i) {
        forkValue = fork();
        if (forkValue == 0)
            placePawn(&pawnArray[i]);
    }

    playerHandler();
}

