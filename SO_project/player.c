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

extern int shmId;

void playerBirth(int pawnNumber) {
    table *sharedTable;
    //sleep(10);

    fprintf(stderr, "%d", shmId);
    sharedTable = shmat(shmId, NULL, 0);
    test_error();
    shmdt(sharedTable);
    exit(0);
}

