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
/*#define DEBUG*/

/*IPCs*/
int shmId, playerSem, flagShm, flagNum, roundStartSem, pawnMoveSem, flagQueue, indicationSem, broadcastQueue, scoreQueue;
struct timespec thold;
/*game environment*/
env environment;
table *sharedTable;
pid_t *players;
flag *flags;
/*score*/
int *playerScore, *playerMoves, *lastRoundPlayerMoves;
int rounds = 0, roundsTime = 0;


table *tableCreation(int base, int height) {
    table *myTable;
    int i, j;
    int shmTemp;

    /*shared memory creation*/
    shmId = shmget(IPC_PRIVATE, sizeof(table), 0600);
    TEST_ERROR;
    myTable = shmat(shmId, NULL, 0);
    TEST_ERROR;
    /*game table definition*/
    myTable->base = base;
    myTable->height = height;
    shmTemp = shmget(IPC_PRIVATE, sizeof(char) * height, 0600);
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

void printState(table myTable, int end) {
    int i, j, k;
    int movesLeft;

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
    if (!end) {
        printf("\n\n");
        printf("player\tpid\tscore\tmoves left\n");
        for (i = 0; i < environment.SO_NUM_G; ++i) {
            movesLeft = environment.SO_N_MOVES * environment.SO_NUM_P - (playerMoves[i] - lastRoundPlayerMoves[i]);
            printf("%d\t%d\t%d\t%d\n", i, players[i], playerScore[i], movesLeft);
            lastRoundPlayerMoves[i] = movesLeft;
        }
    }

}

void flagsPositioning(table *gameTable, int minFlag, int maxFlag, int roundScore) {
    int flagNotValued;
    int X, Y, positionOccupied;
    int i;

    srand(getpid());
    flagNotValued = flagNum = ((rand() % (maxFlag - (minFlag - 1))) + minFlag);

    for (i = 0; i < flagNum; ++i) {
        /*flag value randomly assigned*/
        flags[i].value = (rand() % (roundScore - flagNotValued--) + 1);
        roundScore -= flags[i].value;
        flags[i].taken = 0;
        flags[i].id = i + 1;

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
        gameTable->matrix[Y][X] = i + 49;
    }
}

void fieldInit(table myTable) {
    int i, j;

    for (i = 0; i < myTable.height; ++i) {
        for (j = 0; j < myTable.base; ++j) {
            myTable.matrix[i][j] = ' ';
        }
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
    msgctl(broadcastQueue, IPC_RMID, NULL);
    shmctl(shmId, IPC_RMID, NULL);
    shmctl(flagShm, IPC_RMID, NULL);
    shmdt(flags);
    shmdt(sharedTable->semMatrix);
    shmdt(sharedTable->matrix);
    shmdt(sharedTable);

}

void alarmHandler() {
    int totScore = 0, i = 0;
    float pointOverTime;

    endGame(environment.SO_NUM_G);
    while (wait(NULL) != -1);
    printState(*sharedTable, 1);
    for (i = 0; i < environment.SO_NUM_G; ++i) {
        totScore += playerScore[i];
    }
    pointOverTime = (float) totScore / (float) (roundsTime + 3);
    printf("round played: %d\ntotal points/gametime %f\n", rounds, pointOverTime);
    printf("player\tpid\tmoves used/moves total\t points/moves \n");
    for (i = 0; i < environment.SO_NUM_G; ++i) {
        printf("%d\t%d\t%f\t\t%f\n", i, players[i],
               (float) playerMoves[i] / (environment.SO_N_MOVES * environment.SO_NUM_P * rounds),
               (float) playerScore[i] / (float) playerMoves[i]);
    }
    clean();

    exit(0);
}

int main(int argc, char **argv) {
    struct sigaction sa;
    int debug = 0;
    int i, j;
    msgFlag message;
    msgScore score;

    /*sigaction setting*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler = alarmHandler;
    sigaction(SIGALRM, &sa, NULL);

    /*setting table*/
    setvbuf(stdout, NULL, _IONBF, 0);
    envReading(environ, &environment);
    sharedTable = tableCreation(environment.SO_BASE, environment.SO_ALTEZZA);
    flagShm = shmget(IPC_PRIVATE, sizeof(flag) * environment.SO_FLAG_MAX, 0600);
    flags = shmat(flagShm, NULL, 0);
    scoreQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    flagsPositioning(sharedTable, environment.SO_FLAG_MIN, environment.SO_FLAG_MAX, environment.SO_ROUND_SCORE);

    playerScore = malloc(sizeof(int) * environment.SO_NUM_G);
    playerMoves = malloc(sizeof(int) * environment.SO_NUM_G);
    lastRoundPlayerMoves = malloc(sizeof(int) * environment.SO_NUM_G);
    for (i = 0; i < environment.SO_NUM_G; ++i) {
        playerMoves[i] = playerScore[i] = lastRoundPlayerMoves[i] = 0;
    }
    thold.tv_nsec = environment.SO_MIN_HOLD_NSEC;
    thold.tv_sec = 0;

    /*creating a semaphore for starting round before the creation of the players, otherwise they may run before the master and
     * start the game before the master is ready*/
    roundStartSem = semget(IPC_PRIVATE, 1, 0600);
    semctl(roundStartSem, 0, SETVAL, environment.SO_NUM_G);
    indicationSem = semget(IPC_PRIVATE, 1, 0600);
    semctl(indicationSem, 0, SETVAL, 0);
    flagQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    broadcastQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    /*creating a semaphore for moving pawn*/
    pawnMoveSem = semget(IPC_PRIVATE, 1, 0600);
    playersCreation(environment.SO_NUM_G, environment.SO_NUM_P);


    while (1) {
        /*setting a semaphore to prevent the pawn from moving
         * no need to set it before creation because of the wait on msg queue*/
        semctl(pawnMoveSem, 0, SETVAL, 1);
        /*semaphore to start the indication phase*/
        /*sleep(10);*/
        semctl(indicationSem, 0, SETVAL, environment.SO_NUM_G);
        debug++;
        /*waiting for the plgayers*/
        semHandling(roundStartSem, 0, 0);
        rounds++;
        alarm(environment.SO_MAX_TIME);
        semHandling(pawnMoveSem, 0, -1); /*round started*/
        for (i = 0; i < flagNum; ++i) {
            msgrcv(flagQueue, &message, sizeof(int) * 2, 0, 0);
            fprintf(stderr, "flag %d received\n", message.id);
            /*il master deve ritrasmettere a tutti sun una coda separata i messaggi ricevuti da questa
             * messaggi da leggere con il flag MSG_COPY*/
            msgsnd(broadcastQueue, &message, sizeof(int) * 2, 0);
            /*fai i tuoi inutili calcoli sui punteggi */
            for (j = 0; j < environment.SO_NUM_G; ++j) {
                if (players[j] == message.playerPid) {
                    playerScore[j] += flags[message.id - 1].value;
                }
            }
        }
        printf("all flags taken\n");
        roundsTime = alarm(0);
        semctl(pawnMoveSem, 0, SETVAL, 1);
        /*empty the broadcast queue*/
        i = 0;
        while (msgrcv(broadcastQueue, &message, sizeof(int) * 2, 0, IPC_NOWAIT) != -1) {
            i++;
        };
        printf("%d", i);
        for (i = 0; i < environment.SO_NUM_G; ++i) {
            kill(players[i], SIGUSR2);
        }
        for (i = 0; i < environment.SO_NUM_G; ++i) {
            msgrcv(scoreQueue, &score, sizeof(int), 0, 0);
            for (j = 0; j < environment.SO_NUM_G; ++j) {
                if (players[j] == score.playerPid) {
                    playerMoves[j] = score.movesUsed;
                }
            }
        }
        printState(*sharedTable, 0);
        /* tu rimani in attesa di messaggi(aka le flag prese)
         * quando tutte le flag sono state prese, riavvia il ciclo, superati i SO_MAX_TIME secondi parte l'handler
         * */
        flagsPositioning(sharedTable, environment.SO_FLAG_MIN, environment.SO_FLAG_MAX,
                         environment.SO_ROUND_SCORE);
        semctl(roundStartSem, 0, SETVAL, environment.SO_NUM_G);
        fieldInit(*sharedTable);
    }
    return 0;
}
