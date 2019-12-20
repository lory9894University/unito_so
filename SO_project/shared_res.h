#ifndef SO_PROJECT_SHARED_RES_H
#define SO_PROJECT_SHARED_RES_H
#define RESERVE -1
#define RELEASE 1
#define _GNU_SOURCE

typedef struct {
    int base, height;
    char **matrix;
    int *semMatrix;
} table;

typedef struct {
    int value;
    int xPos, yPos;
    int taken;
    int id;
} flag;

typedef struct {
    long mtype;
    int id;
    int playerPid;
} msgFlag;

/*semOp, 1 release, -1 reserve*/
int semHandling(int semId, int semNum, int semOp);

#endif //SO_PROJECT_SHARED_RES_H