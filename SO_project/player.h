#ifndef SO_PROJECT_PLAYER_H
#define SO_PROJECT_PLAYER_H

#include "pawn.h"

#define _GNU_SOURCE

pawn *playerBirth(int pawnNumber, int numPlayer, int playersTot, int pawnSem);

void playerLife(pawn *pawnArray, int pawnNumber);

#endif //SO_PROJECT_PLAYER_H