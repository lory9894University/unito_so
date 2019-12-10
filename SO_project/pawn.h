#ifndef SO_PROJECT_PAWN_H
#define SO_PROJECT_PAWN_H
#define _GNU_SOURCE

#include <signal.h>

typedef struct {
    int positionX, positionY;
    int objectiveX, objectiveY;
    pid_t pid;

} pawn;

pid_t createPawn(int posX, int posY);

void pawnLife();

#endif //SO_PROJECT_PAWN_H
