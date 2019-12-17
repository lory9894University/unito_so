#ifndef SO_PROJECT_PLAYER_H
#define SO_PROJECT_PLAYER_H

#include "pawn.h"

#define _GNU_SOURCE

pawn *playerBirth(int pawnNum, int numPlayer, int playersTot, int pawnSem, int moves);

void playerLife(int moves);

#endif //SO_PROJECT_PLAYER_H