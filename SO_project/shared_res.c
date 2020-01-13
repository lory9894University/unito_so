#define _GNU_SOURCE

#include <sys/sem.h>
#include "shared_res.h"

/*semOp, 1 release, -1 reserve*/
int semHandling(int semId, int semNum, int semOp) {
    struct sembuf ops;

    ops.sem_num = semNum;
    ops.sem_op = semOp;
    ops.sem_flg = 0;
    return semop(semId, &ops, 1);
}

int semHandlingTimed(int semId, int semNum, int semOp) {
    struct sembuf ops;
    struct timespec ts;

    ts.tv_sec = 0;
    ts.tv_nsec = 100000000;/*0.1 sec*/
    ops.sem_num = semNum;
    ops.sem_op = semOp;
    ops.sem_flg = 0;
    return semtimedop(semId, &ops, 1, &ts);
}