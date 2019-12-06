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
#ifdef DEBUG
    printf("pawn: game finished, exiting\n");
#endif
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

    sharedTable = shmat(shmId, NULL, 0);
    test_error();
    printf("%p\n", sharedTable);


    thisPawn.positionX = posX;
    thisPawn.positionY = posY;
    thisPawn.pid = getpid();

    sharedTable->matrix[posY][posX] = 'T';
#ifdef DEBUG
    pawnHandler();
#endif
    return thisPawn.pid;
}

