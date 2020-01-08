#define _GNU_SOURCE
#include "pawn.h"
#include "shared_res.h"
#include "error_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>

/*#define DEBUG*/

extern int shmId, flagShm;
extern int pawnMoveSem, msgPawn, flagQueue, broadcastQueue;
struct timespec thold;
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
    if (directives.newDirectives.objectiveId != directives.newDirectives.objective2Id && reachable && directives.newDirectives.objectiveId != -1) {
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
    sharedTable->matrix[directives.newDirectives.positionY][directives.newDirectives.positionX] = 'T';
}

void createPawn(int posX, int posY) {
    struct sigaction sa, se;

    /*sigaction setting*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler = pawnHandler;
    sigaction(SIGUSR1, &sa, NULL);

    bzero(&se, sizeof(se));
    se.sa_handler = roundEnded;
    sigaction(SIGUSR2, &se, NULL);

#ifdef DEBUG
    fprintf(stderr, "pid: %d , i'm a pawn. created by %d\n", getpid(), getppid());
    /*sleep(10); /*thanks debugger*/
#endif
    fprintf(stderr, "pid: %d , i'm a pawn. created by %d\n", getpid(), getppid());
    sharedTable = shmat(shmId, NULL, 0);
    TEST_ERROR;
    flags = shmat(flagShm, NULL, 0);
    sharedTable->matrix[posY][posX] = 'T';
    semHandling(sharedTable->semMatrix[posY], posX, RESERVE);
}

void moving() {
    int yVect, xVect;
    msgFlag message;
    int handlingReturn;
    directives.newDirectives.movesUsed = 0;
    while (directives.newDirectives.movesLeft > 0 && directives.newDirectives.stop==0) {
        #ifdef DEBUG
        sharedTable->matrix[directives.newDirectives.positionY][directives.newDirectives.positionX] = ' ';
        printf("%d ", directives.newDirectives.movesLeft);
        #endif
        /*wait on flagQueue with IPC_NOWAIT, MSG_COPY and msgtype = directives.new.id, while you move*/
        msgrcv(broadcastQueue, &message, sizeof(int) * 2, directives.newDirectives.objectiveId, IPC_NOWAIT | MSG_COPY);
        if (errno != ENOMSG) {
            tookMyFlag();
        }
        yVect = directives.newDirectives.objectiveY - directives.newDirectives.positionY;
        xVect = directives.newDirectives.objectiveX - directives.newDirectives.positionX;
        if (abs(xVect) + abs(yVect) > directives.newDirectives.movesLeft) {
            tookMyFlag();
            yVect = directives.newDirectives.objectiveY - directives.newDirectives.positionY;
            xVect = directives.newDirectives.objectiveX - directives.newDirectives.positionX;
        }
        /*flag not reachable*/
        if (directives.newDirectives.objectiveId == -1)
            break;
        if (yVect == 0 && xVect == 0) {
            /*fprintf(stderr, "flag %d taken\n", directives.newDirectives.objectiveId);*/
            message.id = directives.newDirectives.objectiveId;
            message.playerPid = getppid();
            message.mtype = message.id;
            msgsnd(flagQueue, &message, sizeof(int) * 2, 0);
            /*to avoid waisting CPU time the pawn now change objective without waiting master broadcast
             * other implementation could be taking the flag and waiting the message from the master saying that the flag is taken
             * but this pawn already knows that*/
            tookMyFlag();
        } else {
            if (abs(yVect) < abs(xVect)) {
                /*move on X Axis*/
                if (xVect < 0) {
                    handlingReturn = semHandlingTimed(sharedTable->semMatrix[directives.newDirectives.positionY],
                                                      directives.newDirectives.positionX - 1, RESERVE);
                    if (handlingReturn == -1 && errno == EAGAIN) {
                        /*move on Y Axis*/
                        if (yVect < 0) {
                            handlingReturn = semHandlingTimed(
                                    sharedTable->semMatrix[directives.newDirectives.positionY - 1],
                                    directives.newDirectives.positionX, RESERVE);
                            if (handlingReturn == -1 && errno == EAGAIN) {
                                directives.newDirectives.movesLeft++;
                            } else {
                                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                            directives.newDirectives.positionX, RELEASE);
                                nanosleep(&thold, NULL);
                                directives.newDirectives.positionY--;
                            }
                        } else if (yVect > 0) {
                            handlingReturn = semHandlingTimed(
                                    sharedTable->semMatrix[directives.newDirectives.positionY + 1],
                                    directives.newDirectives.positionX, RESERVE);
                            if (handlingReturn == -1 && errno == EAGAIN) {
                                directives.newDirectives.movesLeft++;
                            } else {
                                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                            directives.newDirectives.positionX, RELEASE);
                                nanosleep(&thold, NULL);
                                directives.newDirectives.positionY++;
                            }
                        }
                    } else {
                        semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                    directives.newDirectives.positionX, RELEASE);
                        nanosleep(&thold, NULL);
                        directives.newDirectives.positionX--;
                    }
                } else {
                    handlingReturn = semHandlingTimed(sharedTable->semMatrix[directives.newDirectives.positionY],
                                                      directives.newDirectives.positionX + 1, RESERVE);
                    if (handlingReturn == -1 && errno == EAGAIN) {
                        /*move on Y Axis*/
                        if (yVect < 0) {
                            handlingReturn = semHandlingTimed(
                                    sharedTable->semMatrix[directives.newDirectives.positionY - 1],
                                    directives.newDirectives.positionX, RESERVE);
                            if (handlingReturn == -1 && errno == EAGAIN) {
                                directives.newDirectives.movesLeft++;
                            } else {
                                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                            directives.newDirectives.positionX, RELEASE);
                                nanosleep(&thold, NULL);
                                directives.newDirectives.positionY--;
                            }
                        } else if (yVect > 0) {
                            handlingReturn = semHandlingTimed(
                                    sharedTable->semMatrix[directives.newDirectives.positionY + 1],
                                    directives.newDirectives.positionX, RESERVE);
                            if (handlingReturn == -1 && errno == EAGAIN) {
                                directives.newDirectives.movesLeft++;
                            } else {
                                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                            directives.newDirectives.positionX, RELEASE);
                                nanosleep(&thold, NULL);
                                directives.newDirectives.positionY++;
                            }
                        }
                    } else {
                        semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                    directives.newDirectives.positionX, RELEASE);
                        nanosleep(&thold, NULL);
                        directives.newDirectives.positionX++;
                    }
                }
        } else {
                /*move on Y Axis*/
                if (yVect < 0) {
                    handlingReturn = semHandlingTimed(sharedTable->semMatrix[directives.newDirectives.positionY - 1],
                                                      directives.newDirectives.positionX, RESERVE);
                    if (handlingReturn == -1 && errno == EAGAIN) {
                        /*move on X Axis*/
                        if (xVect < 0) {
                            handlingReturn = semHandlingTimed(
                                    sharedTable->semMatrix[directives.newDirectives.positionY],
                                    directives.newDirectives.positionX - 1, RESERVE);
                            if (handlingReturn == -1 && errno == EAGAIN) {
                                /*send message stuck*/
                                directives.newDirectives.movesLeft++;
                            } else {
                                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                            directives.newDirectives.positionX, RELEASE);
                                nanosleep(&thold, NULL);
                                directives.newDirectives.positionX--;
                            }
                        } else {
                            handlingReturn = semHandlingTimed(
                                    sharedTable->semMatrix[directives.newDirectives.positionY],
                                    directives.newDirectives.positionX + 1, RESERVE);
                            if (handlingReturn == -1 && errno == EAGAIN) {
                                /*todo:send message stuck*/
                                directives.newDirectives.movesLeft++;
                            } else {
                                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                            directives.newDirectives.positionX, RELEASE);
                                nanosleep(&thold, NULL);
                                directives.newDirectives.positionX++;
                            }
                        }
                    } else {
                        semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                    directives.newDirectives.positionX, RELEASE);
                        nanosleep(&thold, NULL);
                        directives.newDirectives.positionY--;
                    }
                } else {
                    handlingReturn = semHandlingTimed(sharedTable->semMatrix[directives.newDirectives.positionY + 1],
                                                      directives.newDirectives.positionX, RESERVE);
                    if (handlingReturn == -1 && errno == EAGAIN) {
                        /*move on X Axis*/
                        if (xVect < 0) {
                            handlingReturn = semHandlingTimed(
                                    sharedTable->semMatrix[directives.newDirectives.positionY],
                                    directives.newDirectives.positionX - 1, RESERVE);
                            if (handlingReturn == -1 && errno == EAGAIN) {
                                /*send message stuck*/
                                directives.newDirectives.movesLeft++;
                            } else {
                                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                            directives.newDirectives.positionX, RELEASE);
                                nanosleep(&thold, NULL);
                                directives.newDirectives.positionX--;
                            }
                        } else {
                            handlingReturn = semHandlingTimed(
                                    sharedTable->semMatrix[directives.newDirectives.positionY],
                                    directives.newDirectives.positionX + 1, RESERVE);
                            if (handlingReturn == -1 && errno == EAGAIN) {
                                /*todo:send message stuck*/
                                directives.newDirectives.movesLeft++;
                            } else {
                                semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                            directives.newDirectives.positionX, RELEASE);
                                nanosleep(&thold, NULL);
                                directives.newDirectives.positionX++;
                            }
                        }
                    } else {
                        semHandling(sharedTable->semMatrix[directives.newDirectives.positionY],
                                    directives.newDirectives.positionX, RELEASE);
                        nanosleep(&thold, NULL);
                        directives.newDirectives.positionY++;
                    }
                }
            }
            directives.newDirectives.movesLeft--;
            directives.newDirectives.movesUsed++;

        }
#ifdef DEBUG
        sharedTable->matrix[directives.newDirectives.positionY][directives.newDirectives.positionX] = 'T';
#endif
    }

}

void pawnLife() {
    int debugReturnSemop;
    int msgReturn;

    while (1) {
        /*wait for instructions from player*/
        msgReturn = msgrcv(msgPawn, &directives, sizeof(pawn), getpid(), 0);
        while (errno == EINTR && msgReturn == -1) {
            /*fprintf(stderr, "deep shit pawn\n");*/
            msgReturn = msgrcv(msgPawn, &directives, sizeof(pawn), getpid(), 0);
        }
        debugReturnSemop = semHandling(pawnMoveSem, 0, 0);
        while (errno == EINTR && debugReturnSemop == -1) {
            debugReturnSemop = semHandling(pawnMoveSem, 0, 0);
        } /*wait for master to start the round*/
        if (debugReturnSemop) {
            perror("error on waiting on pawnMoveSem");
        }
#ifdef DEBUG
        fprintf(stderr, "%d directives. posx: %d posy %d, dirx: %d diry: %d\n", getpid(),
                directives.newDirectives.positionX, directives.newDirectives.positionY,
                directives.newDirectives.objectiveX, directives.newDirectives.objectiveY);
#endif
        sharedTable->matrix[directives.newDirectives.positionY][directives.newDirectives.positionX] = ' ';
        moving();
        /*when pawn finished moving or when end round signal received pawn sends his coordinates to player*/
#ifdef DEBUG
        if (directives.newDirectives.movesLeft==0)
            fprintf(stderr, "finito le mosse\n");
#endif
        directives.mtype = 1;
        msgsnd(msgPawn, &directives, sizeof(pawn), 0);
    }
}
