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
#include <sys/msg.h>
#include "env_operation.h"
#include "player.h"
#include "shared_res.h"
#include "pawn.h"
//#define DEBUG

int shmId, playerSem, flagShm, flagNum, roundStartSem, pawnMoveSem, flagQueue, indicationSem;
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
    shmTemp = shmget(IPC_PRIVATE, sizeof(char) * height, 0600); //todo: questi sono char non int
    myTable->matrix = shmat(shmTemp, NULL, 0);
    shmctl(shmTemp, IPC_RMID, NULL);
    shmTemp = shmget(IPC_PRIVATE, sizeof(char) * height, 0600);
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

void flagsPositioning(table *gameTable, int minFlag, int maxFlag, int roundScore) {
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
        flags[i].id = i;

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

void playersCreation(int numPlayers, int numPawn) {
    int i = 0, forkVal = -1;
    int placePawnSem;

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
            playerBirth(numPawn, i, numPlayers, placePawnSem, environment.SO_N_MOVES);
        } else /*father actions*/
            players[i] = forkVal;
    }
    if (forkVal == 0) {
        playerLife(environment.SO_N_MOVES);
    }

    /*wait for every player to place his pawns*/
    semHandling(placePawnSem, 0, 0);
    semctl(placePawnSem, 0, IPC_RMID);
}

void endGame(int numPlayers) {
    int i;
    for (i = 0; i < numPlayers; ++i) {
        kill(players[i], SIGUSR1);
    }
}

void clean() {
    int i;

    semctl(roundStartSem, 0, IPC_RMID);
    semctl(pawnMoveSem, 0, IPC_RMID);
    semctl(indicationSem, 0, IPC_RMID);

    for (i = 0; i < environment.SO_NUM_G; ++i) {
        semctl(playerSem, 0, IPC_RMID);
    }
    for (i = 0; i < environment.SO_ALTEZZA; ++i) {
        semctl(sharedTable->semMatrix[i], 0, IPC_RMID);
        shmdt(sharedTable->matrix[i]);
    }
    msgctl(flagQueue, IPC_RMID, NULL);
    shmctl(shmId, IPC_RMID, NULL);
    shmdt(sharedTable->semMatrix);
    shmdt(sharedTable->matrix);
    shmdt(sharedTable);

}

void alarmHandler() {

    endGame(environment.SO_NUM_G);
    while (wait(NULL) != -1);
    printState(*sharedTable);
    clean();

    exit(0);
}

int main(int argc, char **argv) {
    struct sigaction sa;
    int debug = 0;
    int i;
    msgFlag message;


    /*sigaction setting*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler = alarmHandler;
    sigaction(SIGALRM, &sa, NULL);

    /*setting table*/
    setvbuf(stdout, NULL, _IONBF, 0);
    envReading(environ, &environment);
    sharedTable = tableCreation(environment.SO_BASE, environment.SO_ALTEZZA);
    flagsPositioning(sharedTable, environment.SO_FLAG_MIN, environment.SO_FLAG_MAX, environment.SO_ROUND_SCORE);

    /*creating a semaphore for starting round before the creation of the players, otherwise they may run before the master and
     * start the game before the master is ready*/
    roundStartSem = semget(IPC_PRIVATE, 1, 0600);
    semctl(roundStartSem, 0, SETVAL, environment.SO_NUM_G);
    indicationSem = semget(IPC_PRIVATE, 1, 0600);
    semctl(indicationSem, 0, SETVAL, 0);
    playersCreation(environment.SO_NUM_G, environment.SO_NUM_P);

    flagQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    /*creating a semaphore for moving pawn*/
    pawnMoveSem = semget(IPC_PRIVATE, 1, 0600);
    TEST_ERROR


    while (debug < 1) {
        /*setting a semaphore to prevent the pawn from moving
         * no need to set it before creation because of the wait on msg queue*/
        semctl(pawnMoveSem, 0, SETVAL, 1);
        /*semaphore to start the indication phase*/
        //sleep(10);
        semctl(indicationSem, 0, SETVAL, environment.SO_NUM_G);
        debug++;
        TEST_ERROR
        /*waiting for the players*/
        semHandling(roundStartSem, 0, 0);
        alarm(10);
        semHandling(pawnMoveSem, 0, -1); /*round started*/
        for (i = 0; i < flagNum; ++i) {
            sleep(1);
            msgrcv(flagQueue, &message, sizeof(int) * 2, 0, 0);
            /*il master deve ritrasmettere a tutti sun una coda separata i messaggi ricevuti da questa
             * messaggi da leggere con il flag MSG_COPY*/
            /*fai i tuoi inutili calcoli sui punteggi */

        }
        for (i = 0; i < environment.SO_NUM_G; ++i) {
            kill(players[i], SIGUSR2);
        }
        /* tu rimani in attesa di messaggi(aka le flag prese)
         * quando tutte le flag sono state prese, riavvia il ciclo, superati i SO_MAX_TIME secondi parte l'handler
         * */
        sleep(7);
        alarm(0);
        flagsPositioning(sharedTable, environment.SO_FLAG_MIN, environment.SO_FLAG_MAX,
                         environment.SO_ROUND_SCORE);
        semctl(roundStartSem, 0, SETVAL, environment.SO_NUM_G);
    }

    return 0;
}
