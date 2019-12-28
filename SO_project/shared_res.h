#ifndef SO_PbeaROJECT_SHARED_RES_H
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

typedef struct {
    long movesUsed;
    int playerPid;
} msgScore;

/*semaphore Operations*/
/*semOp, 1 release, -1 reserve*/
int semHandling(int semId, int semNum, int semOp);

int semHandlingTimed(int semId, int semNum, int semOp);

/*list operation*/
typedef struct node *syncNode;
struct node {
    int distance;
    int semIndex;

    syncNode next;
};

syncNode newNode(syncNode next);

void freeList(syncNode head);

void insertList(syncNode *head, int distance, int semIndex);

void changePriorityList(syncNode *head, int newDistance); /*only the head node will be changed*/

#endif /*SO_PROJECT_SHARED_RES_H*/