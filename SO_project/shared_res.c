#define _GNU_SOURCE

#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error_handling.h"
#include "shared_res.h"

/*semOp, 1 release, -1 reserve*/
semHandling(int semId, int semNum, int semOp) {
    struct sembuf ops;

    ops.sem_num = semNum;
    ops.sem_op = semOp;
    ops.sem_flg = 0;
    return semop(semId, &ops, 1);
}
