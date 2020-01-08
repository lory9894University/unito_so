#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <strings.h>
#include <sys/shm.h>
#include "shared_res.h"
#include "player.h"
#include "error_handling.h"
#include "pawn.h"

/*#define DEBUG*/

extern int shmId;
extern table *sharedTable;
extern int playerSem, roundStartSem, indicationSem;
int msgPawn, pawnNumber;
pawn *pawnArray;

void playerHandler(int signum) {
    int i;
#ifdef DEBUG
    int childPid = 0;
    int status = 0;
    printf("%d player: game finished, waiting child and exiting\n", getpid());
#endif
    for (i = 0; i < pawnNumber; ++i) {
        kill(pawnArray[i].pid, SIGUSR1);
    }
#ifdef DEBUG
    while ((childPid = wait(&status)) != -1)
        dprintf(2, "PID=%d. Sender (PID=%d) terminated with status %d\n", getpid(), childPid, WEXITSTATUS(status));
#else
    while (wait(NULL) != -1);
#endif
    for (i = 0; i < sharedTable->height; ++i) {
        shmdt(sharedTable->matrix[i]);
    }
    shmdt(sharedTable->semMatrix);
    shmdt(sharedTable->matrix);
    shmdt(sharedTable);
    msgctl(msgPawn, IPC_RMID, NULL);
    exit(0);
}

void endRound(int signum) {
    int i;
    for (i = 0; i < pawnNumber; ++i) {
        kill(pawnArray[i].pid, SIGUSR2);
    }
}

pawn *playerBirth(int pawnNum, int numPlayer, int playersTot, int pawnSem, int moves) {
    int i;
    struct sigaction sa, sf;
    int forkValue = -1;
    int posX = 0, posY = 0, positionOccupied = 1;
    pawnNumber = pawnNum;

    /*sigaction setting*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler = playerHandler;
    sigaction(SIGUSR1, &sa, NULL);

    bzero(&sf, sizeof(sf));
    sf.sa_handler = endRound;
    sf.sa_flags = SA_RESTART;
    sigaction(SIGUSR2, &sf, NULL);

#ifdef DEBUG
    fprintf(stderr, "pid: %d , i'm a player\n", getpid());
    /*sleep(10); /*thanks debugger*/
    /*printf("%d ", shmId);*/
#endif
    sharedTable = shmat(shmId, NULL, 0);
    /*TODO: questo costrutto è veramente necessario? il puntatore a sharedTable è già nell'heap*/
    TEST_ERROR;

    msgPawn = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    TEST_ERROR;
    /*pawn creation*/
    pawnArray = malloc(sizeof(pawn) * pawnNumber);
    srand(getpid());
    for (i = 0; i < pawnNumber && forkValue != 0; ++i) {
        semHandling(playerSem, numPlayer, RESERVE);
        TEST_ERROR
        positionOccupied = 1;
        while (positionOccupied) {
            posX = rand() % sharedTable->base;
            posY = rand() % sharedTable->height;
            if (sharedTable->matrix[posY][posX] == ' ')
                positionOccupied = 0;
        }
        forkValue = fork();
        if (forkValue == 0) {
            createPawn(posX, posY);
            pawnLife();
        } else {
            pawnArray[i].pid = forkValue;
            pawnArray[i].positionY = posY;
            pawnArray[i].positionX = posX;
            pawnArray[i].objectiveX = -1;
            pawnArray[i].objectiveY = -1;
            pawnArray[i].objective2X = -1;
            pawnArray[i].objective2Y = -1;
            pawnArray[i].movesLeft = moves;
            pawnArray[i].stop = 0;
        }
        semHandling(playerSem, numPlayer == playersTot - 1 ? 0 : numPlayer + 1, RELEASE);
        TEST_ERROR
    }

    semHandling(pawnSem, 0, RESERVE);
    return pawnArray;
}

void objectives(flag *flags) {
    int i, j;
    int bestFlag = -1, secondBestFlag = -1, bestPawn = -1, distanceBest, distanceLocal = 0;
    int *flagObjective;
    extern int flagNum;
    pawnDirection directives;

    flagObjective = malloc(sizeof(int) * flagNum);
    for (i = 0; i < flagNum; ++i) {
        flagObjective[i] = 0;
    }
    /*closest flag to every pawn*/
    for (i = 0; i < pawnNumber; ++i) {
        distanceBest = 0;
        distanceLocal = 0;
        for (j = 0; j < flagNum; ++j) {
            distanceLocal = abs(pawnArray[i].positionX - flags[j].xPos) + abs(pawnArray[i].positionY - flags[j].yPos);
            if ((distanceLocal < distanceBest || distanceBest == 0)) {
                distanceBest = distanceLocal;
                bestFlag = j;
            }
        }
        distanceBest = 0;
        distanceLocal = 0;
        for (j = 0; j < flagNum; ++j) {
            distanceLocal = abs(pawnArray[i].positionX - flags[j].xPos) + abs(pawnArray[i].positionY - flags[j].yPos);
            if ((distanceLocal < distanceBest || distanceBest == 0) && j != bestFlag) {
                distanceBest = distanceLocal;
                secondBestFlag = j;
            }
        }
        flagObjective[bestFlag] += 1;
        pawnArray[i].objectiveX = flags[bestFlag].xPos;
        pawnArray[i].objectiveY = flags[bestFlag].yPos;
        pawnArray[i].objectiveId = flags[bestFlag].id;
        pawnArray[i].objective2X = flags[secondBestFlag].xPos;
        pawnArray[i].objective2Y = flags[secondBestFlag].yPos;
        pawnArray[i].objective2Id = flags[secondBestFlag].id;
    }
    /* closest pawn to the flags not taken*/
    distanceBest = 0;
    distanceLocal = 0;
    for (i = 0; i < flagNum; ++i) {
        if (flagObjective[i] == 0) {
            for (j = 0; j < pawnNumber; ++j) {
                distanceLocal =
                        abs(pawnArray[j].positionX - flags[i].xPos) + abs(pawnArray[j].positionY - flags[i].yPos);
                if ((distanceLocal < distanceBest || distanceBest == 0)) {
                    distanceBest = distanceLocal;
                    bestPawn = j;
                    flagObjective[i] = 1;
                }
            }
            pawnArray[bestPawn].objectiveX = flags[i].xPos;
            pawnArray[bestPawn].objectiveY = flags[i].yPos;
            flagObjective[pawnArray[bestPawn].objectiveId - 1] -= 1;
            pawnArray[bestPawn].objectiveId = flags[i].id;
            i = 0;
        }
    }
    for (i = 0; i < pawnNumber; ++i) {
        directives.mtype = pawnArray[i].pid;
        directives.newDirectives = pawnArray[i];
        msgsnd(msgPawn, &directives, sizeof(pawn), 0);
    }
    fprintf(stderr, "\n");
}

void playerLife(int moves) {
    flag *flags;
    extern int flagShm, scoreQueue;
    int i = 0, j;
    pawnDirection direction;
    int semandlingReturn;
    msgScore score;


    flags = shmat(flagShm, NULL, 0);
    /*sleep(10);*/
    while (1) {
        score.movesUsed = 0;
        semandlingReturn = semHandling(indicationSem, 0, RESERVE);
        while (errno == EINTR && semandlingReturn == -1) {
            semandlingReturn = semHandling(indicationSem, 0, RESERVE);
        }
        /*fprintf(stderr, "started indication\n");*/
        objectives(flags);
        semHandling(roundStartSem, 0, -1);
#ifdef DEBUG
        printf("master can start\n");
#endif
        for (i = 0; i < pawnNumber; ++i) {
            msgrcv(msgPawn, &direction, sizeof(pawn), 1, 0);
            for (j = 0; j < pawnNumber; ++j) {
                if (direction.newDirectives.pid == pawnArray[j].pid) {
                    pawnArray[i].positionX = direction.newDirectives.positionX;
                    pawnArray[i].positionY = direction.newDirectives.positionY;
                    pawnArray[i].movesLeft = moves;
                    score.movesUsed += direction.newDirectives.movesUsed;
                    /*printf("%d\n", direction.newDirectives.movesLeft);*/
                }
            }
        }
        score.playerPid = getpid();
        msgsnd(scoreQueue, &score, sizeof(int), 0);
    }
}