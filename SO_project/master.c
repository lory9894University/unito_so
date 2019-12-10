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

int shmId, playerSem, flagShm, flagNum;
env environment;
table *sharedTable;
pid_t *players;

table *tableCreation(int base, int height) {
    table *myTable;
    int i, j;
    int shmTemp;

    /*shared memory creation*/
    shmId = shmget(IPC_PRIVATE, sizeof(table), 0600);
    TEST_ERROR;
    myTable = shmat(shmId, NULL, 0);
    TEST_ERROR;
    //game table definition
    myTable->base = base;
    myTable->height = height;
    shmTemp = shmget(IPC_PRIVATE, sizeof(int) * height, 0600);
    myTable->matrix = shmat(shmTemp, NULL, 0);
    shmctl(shmTemp, IPC_RMID, NULL);
    shmTemp = shmget(IPC_PRIVATE, sizeof(int) * height, 0600);
    myTable->semMatrix = shmat(shmTemp, NULL, 0);
    shmctl(shmTemp, IPC_RMID, NULL);
    for (i = 0; i < height; ++i) {
        shmTemp = shmget(IPC_PRIVATE, sizeof(int) * base, 0600);
        myTable->matrix[i] = shmat(shmTemp, NULL, 0);
        shmctl(shmTemp, IPC_RMID, NULL);
        myTable->semMatrix[i] = semget(IPC_PRIVATE, base, 0600);;
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
    int flagNotValued;
    int X, Y, positionOccupied;
    int i;
    flag *flags;

    srand(getpid());
    flagNotValued = flagNum = ((rand() % (maxFlag - (minFlag - 1))) + minFlag);
    flagShm = shmget(IPC_PRIVATE, sizeof(flag) * flagNum, 0600);
    flags = shmat(flagShm, NULL, 0);

    /*i giocatori devono sapere il valore della bandiera?*/
    for (i = 0; i < flagNum; ++i) {
        /*flag value randomly assigned*/
        flags[i].value = (rand() % (roundScore - flagNotValued--) + 1);
        roundScore -= flags[i].value;
        flags[i].taken = 0;

        /*flag position randomly assigned, if the position is already occupied it calculate a nev position*/
        positionOccupied = 1;
        while (positionOccupied) {
            X = rand() % gameTable->base;
            Y = rand() % gameTable->height;
            positionOccupied = 0;
            if (gameTable->matrix[Y][X] != ' ') {
                positionOccupied = 1;
            }
        }
        flags[i].xPos = X;
        flags[i].yPos = Y;
        gameTable->matrix[Y][X] = 'F';
    }
}

pid_t *playersCreation(int numPlayers, int numPawn) {
    int i = 0, forkVal = -1;
    pid_t *players;
    int placePawnSem;
    pawn *playerPawnArray;

    playerPawnArray = malloc(sizeof(pawn) * numPawn);
    placePawnSem = semget(IPC_PRIVATE, 1, 0600);
    TEST_ERROR
    semctl(placePawnSem, 0, SETVAL, numPlayers);
    TEST_ERROR

    playerSem = semget(IPC_PRIVATE, numPlayers, 0600);
    TEST_ERROR
    for (i = 0; i < numPlayers; ++i) {
        semctl(playerSem, i, SETVAL, i == 0 ? 1 : 0);
        TEST_ERROR
    }
    players = malloc(sizeof(pid_t) * numPlayers);
    for (i = 0; i < numPlayers && forkVal != 0; ++i) {
        forkVal = fork();
        if (forkVal == 0) {
            playerPawnArray = playerBirth(numPawn, i, numPlayers, placePawnSem);
        } else /*father actions*/
            players[i] = forkVal;
    }
    if (forkVal == 0) {
        playerLife(playerPawnArray, numPawn);
    }

    /*wait for every player to place his pawns*/
    semHandling(placePawnSem, 0, 0);
    semctl(placePawnSem, 0, IPC_RMID);
    return players;
}

void endGame(int numPlayers, pid_t *players) {
    int i;
    for (i = 0; i < numPlayers; ++i) {
        kill(players[i], SIGUSR1);
    }
}

void clean(env environment, table *sharedTable, pid_t *players) {
    int i;
    for (i = 0; i < environment.SO_NUM_G; ++i) {
        semctl(playerSem, 0, IPC_RMID);
    }
    for (i = 0; i < environment.SO_ALTEZZA; ++i) {
        semctl(sharedTable->semMatrix[i], 0, IPC_RMID);
        shmdt(sharedTable->matrix[i]);
    }
    shmctl(shmId, IPC_RMID, NULL);
    shmdt(sharedTable->semMatrix);
    shmdt(sharedTable->matrix);
    shmdt(sharedTable);

}

void alarmHandler() {

    endGame(environment.SO_NUM_G, players);
    while (wait(NULL) != -1);
    printState(*sharedTable);
    clean(environment, sharedTable, players);

    exit(0);
}

int main(int argc, char **argv) {
    struct sigaction sa;
    int debug = 0;
    flag *flags;

    /*sigaction setting*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler = alarmHandler;
    sigaction(SIGALRM, &sa, NULL);

    /*setting table*/
    setvbuf(stdout, NULL, _IONBF, 0);
    envReading(environ, &environment);
    sharedTable = tableCreation(environment.SO_BASE, environment.SO_ALTEZZA);
    flags = flagsPositioning(sharedTable, environment.SO_FLAG_MIN, environment.SO_FLAG_MAX, environment.SO_ROUND_SCORE);
    players = playersCreation(environment.SO_NUM_G, environment.SO_NUM_P);


    while (debug < 1) {
        debug++;
        alarm(environment.SO_MAX_TIME);
        /* tu rimani in attesa di messaggi(aka le flag prese)
         * quando tutte le flag sono state prese, riavvia il ciclo, superati i SO_MAX_TIME secondi parte l'handler
         * */
        sleep(4);
    }

    return 0;
}
