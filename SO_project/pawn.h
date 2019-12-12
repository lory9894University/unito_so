#ifndef SO_PROJECT_PAWN_H
#define SO_PROJECT_PAWN_H
#define _GNU_SOURCE

#include <signal.h>

typedef struct {
    int positionX, positionY;
    int objectiveX, objectiveY;
    int objective2X, objective2Y;
    int movesLeft;
    pid_t pid;

} pawn;

typedef struct {
    long mtype;             /* message type */
    pawn newDirectives;
} pawnDirection;

void createPawn(int posX, int posY);

void pawnLife();

#endif //SO_PROJECT_PAWN_H
