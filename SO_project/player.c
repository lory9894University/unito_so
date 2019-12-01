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

extern int shmId;/*processes already have in their stack the id of the shared mem*/

void placePawn() {

}

void playerBirth(int pawnNumber) {
    table *sharedTable;
    int i;
    //sleep(10);
#ifdef DEBUG
    printf("%d ", shmId);
#endif
    sharedTable = shmat(shmId, NULL, 0);
    test_error();

    for (i = 0; i < pawnNumber; ++i) {

    }

    shmdt(sharedTable);
    exit(0);
}

