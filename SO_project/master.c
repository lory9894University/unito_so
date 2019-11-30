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

#define DEBUG

int shmId;

table *tableCreation(int base, int height) {
    table *mytable;
    shmId = shmget(IPC_PRIVATE, sizeof(table), 0660);
    //TODO: come cazzo si usa IPC_EXCL?
    test_error();
    mytable = shmat(shmId, NULL, 0);
    test_error();

    mytable->base = base;
    mytable->height = height;
    return mytable;
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
    table *sharedTable;
    pid_t *players;

    envReading(envp, &environment);
#ifdef DEBUG
    printf("%d", environment.SO_NUM_G);
#endif
    sharedTable = tableCreation(environment.SO_BASE, environment.SO_ALTEZZA);
    //todo: così facendo (creazione delal tabella *prima* della creazione dei figli, essi avvranno nel loro stack 2 puntatori alla memoria, uno praticamente inutile però)
    playersCreation(environment.SO_NUM_G, environment.SO_NUM_G);

    //cleanig what is left
    while (wait(NULL) != -1);
    shmctl(shmId, IPC_RMID, NULL);
    shmdt(sharedTable);
    return 0;
}
