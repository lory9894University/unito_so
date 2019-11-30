#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>
#include "env_operation.h"
#include "player.h"

#define DEBUG
typedef struct {
    int base, height;
} table;

void tableCreation(int base, int height) {
    //TODO: da fare
}

void printState() {

}

void flagsPositioning(table gameTable, int minFlag, int maxFlag, int roundScore) {
    int flagNum;

    srand(getpid());
    flagNum = (rand() % (maxFlag - minFlag) + minFlag);

    //todo: posizionamento delle bandiere sulla matrice schacchiera.
    //i giocatori devono sapere il valore della bandiera?

}

pid_t *playersCreation(int numPlayers, int numPawn) {
    int i = 0, forkVal = -1;
    pid_t *players;

    players = malloc(sizeof(pid_t) * numPlayers);
    for (i = 0; i < numPlayers && forkVal != 0; ++i) {
        forkVal = fork();
        if (forkVal == 0) {
#ifdef DEBUG
            fprintf(stderr, "pid: %d , i'm a child\n", getpid());
#endif
            playerBirth(numPawn);
        } else //father actions
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

    envReading(envp, &environment);
#ifdef DEBUG
    printf("%d", environment.SO_NUM_G);
#endif
    tableCreation(environment.SO_BASE, environment.SO_ALTEZZA);
    playersCreation(environment.SO_NUM_G, environment.SO_NUM_G);


    return 0;
}
