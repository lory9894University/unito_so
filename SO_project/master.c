#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include "env_operation.h"
#include "player.h"
#include "shared_res.h"
#include "pawn.h"

//#define DEBUG

int shmId;

table *tableCreation(int base, int height) {
    table *myTable;
    int i, j;

    /*shared memory creation*/
    shmId = shmget(IPC_PRIVATE, sizeof(table), 0777);
    //TODO: come cazzo si usa IPC_EXCL?
    test_error();
    myTable = shmat(shmId, NULL, 0);
    test_error();
    //game table definition
    myTable->base = base;
    myTable->height = height;
    myTable->matrix = malloc(sizeof(char *) * height);
    myTable->semMatrix = malloc(sizeof(int) * height);
    for (i = 0; i < height; ++i) {
        myTable->matrix[i] = malloc(sizeof(char) * base);
        myTable->semMatrix[i] = semget(IPC_PRIVATE, base, 0777);;
        for (j = 0; j < base; ++j) {
            myTable->matrix[i][j] = ' ';
            semctl(myTable->semMatrix[i], j, SETVAL, 1);
        }
    }

    return myTable;
}

void printState(
        table myTable) { /*TODO:chiedere al prof come faccio a far entrare una tabella di 120 colonne in uno schermo da 15 pollici */
    int i, j, k;

    for (i = 0; i < myTable.base; ++i) {
        printf("----");
    }
    printf("\n");
    for (i = 0; i < myTable.height; ++i) {
        printf("|");
        for (j = 0; j < myTable.base; ++j) {
            printf(" %c |", myTable.matrix[i][j]);
        }
        printf("\n");
        for (k = 0; k < myTable.base; ++k) {
            printf("----");
        }
        printf("\n");
    }
}

flag *flagsPositioning(table *gameTable, int minFlag, int maxFlag, int roundScore) {
    int flagNum, flagNotValued;
    int postionXY, X, Y, positionOccupied;
    int i, j;
    flag *flags;

    srand(getpid());
    flagNotValued = flagNum = ((rand() % (maxFlag - (minFlag - 1))) + minFlag);
    flags = malloc(sizeof(flag) * flagNum);

    /*i giocatori devono sapere il valore della bandiera?*/
    for (i = 0; i < flagNum; ++i) {
        /*flag value randomly assigned*/
        flags[i].value = (rand() % (roundScore - flagNotValued--) + 1);
        roundScore -= flags[i].value;
        flags[i].taken = 0;

        /*flag position randomly assigned, if the position is already occupied it calculate a nev postion*/
        positionOccupied = 1;
        while (positionOccupied) {
            postionXY = (rand() % (gameTable->base * gameTable->height));
            positionOccupied = 0;
            for (j = 0; j < (flagNum - flagNotValued) && !positionOccupied; ++j) {
                if ((flags[j].xPos * flags[j].yPos) == postionXY) {
                    positionOccupied = 1;
                }
            }
        }

        Y = (int) postionXY / gameTable->base;
        X = (int) postionXY % gameTable->base;
        flags[i].xPos = X;
        flags[i].yPos = Y;
        gameTable->matrix[Y][X] = 'F';
    }
}

pid_t *playersCreation(int numPlayers, int numPawn) {
    int i = 0, forkVal = -1;
    pid_t *players;

    players = malloc(sizeof(pid_t) * numPlayers);
    for (i = 0; i < numPlayers && forkVal != 0; ++i) {
        forkVal = fork();
        if (forkVal == 0) {
            playerBirth(numPawn);
        } else /*father actions*/
            players[i] = forkVal;
    }

    return players;
}

void playerKill(int numPlayers, pid_t *players) {
    int i;
    for (i = 0; i < numPlayers; ++i) {
        kill(players[i], SIGKILL);
    }
}

int main(int argc, char **argv, char **envp) {
    env environment;
    table *sharedTable;
    pid_t *players;
    int i;

    setvbuf(stdout, NULL, _IONBF, 0);
    envReading(envp, &environment);
    sharedTable = tableCreation(environment.SO_BASE, environment.SO_ALTEZZA);
    flagsPositioning(sharedTable, environment.SO_FLAG_MIN, environment.SO_FLAG_MAX, environment.SO_ROUND_SCORE);
    //printState(*sharedTable);
    printf("%p\n", sharedTable);
    playersCreation(environment.SO_NUM_G, environment.SO_NUM_P);

    /*cleanig what is left*/
    while (wait(NULL) != -1);
    printState(*sharedTable);
    for (i = 0; i < environment.SO_ALTEZZA; ++i) {
        semctl(sharedTable->semMatrix[i], 0, IPC_RMID);
    }
    shmctl(shmId, IPC_RMID, NULL);
    shmdt(sharedTable);
    return 0;
}
