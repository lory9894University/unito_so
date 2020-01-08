#ifndef SO_PROJECT_ENV_OPERATION_H
#define SO_PROJECT_ENV_OPERATION_H
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include "error_handling.h"

typedef struct {
    int SO_NUM_G;
    int SO_NUM_P;
    int SO_MAX_TIME;
    int SO_BASE;
    int SO_ALTEZZA;
    int SO_FLAG_MIN;
    int SO_FLAG_MAX;
    int SO_ROUND_SCORE;
    int SO_N_MOVES;
    int SO_MIN_HOLD_NSEC;
} env;

void envReading(char **envp, env *environment);

#endif