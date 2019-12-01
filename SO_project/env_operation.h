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

void envReading(char **envp, env *environment) {
    char *SO_NUM_G_str;
    SO_NUM_G_str = getenv("SO_NUM_G");
    if (SO_NUM_G_str == NULL)
        error("environment variable SO_NUM_G not set");
    environment->SO_NUM_G = atoi(SO_NUM_G_str);
    char *SO_NUM_P_str;
    SO_NUM_P_str = getenv("SO_NUM_P");
    if (SO_NUM_P_str == NULL)
        error("environment variable SO_NUM_P not set");
    environment->SO_NUM_P = atoi(SO_NUM_P_str);
    char *SO_MAX_TIME_str;
    SO_MAX_TIME_str = getenv("SO_MAX_TIME");
    if (SO_MAX_TIME_str == NULL)
        error("environment variable SO_MAX_TIME not set");
    environment->SO_MAX_TIME = atoi(SO_MAX_TIME_str);
    char *SO_BASE_str;
    SO_BASE_str = getenv("SO_BASE");
    if (SO_BASE_str == NULL)
        error("environment variable SO_BASE not set");
    environment->SO_BASE = atoi(SO_BASE_str);
    char *SO_ALTEZZA_str;
    SO_ALTEZZA_str = getenv("SO_ALTEZZA");
    if (SO_ALTEZZA_str == NULL)
        error("environment variable SO_ALTEZZA not set");
    environment->SO_ALTEZZA = atoi(SO_ALTEZZA_str);
    char *SO_FLAG_MIN_str;
    SO_FLAG_MIN_str = getenv("SO_FLAG_MIN");
    if (SO_FLAG_MIN_str == NULL)
        error("environment variable SO_FLAG_MIN not set");
    environment->SO_FLAG_MIN = atoi(SO_FLAG_MIN_str);
    char *SO_FLAG_MAX_str;
    SO_FLAG_MAX_str = getenv("SO_FLAG_MAX");
    if (SO_FLAG_MAX_str == NULL)
        error("environment variable SO_FLAG_MAX not set");
    environment->SO_FLAG_MAX = atoi(SO_FLAG_MAX_str);
    char *SO_ROUND_SCORE_str;
    SO_ROUND_SCORE_str = getenv("SO_ROUND_SCORE");
    if (SO_ROUND_SCORE_str == NULL)
        error("environment variable SO_ROUND_SCORE not set");
    environment->SO_ROUND_SCORE = atoi(SO_ROUND_SCORE_str);
    char *SO_N_MOVES_str;
    SO_N_MOVES_str = getenv("SO_N_MOVES");
    if (SO_N_MOVES_str == NULL)
        error("environment variable SO_N_MOVES not set");
    environment->SO_N_MOVES = atoi(SO_N_MOVES_str);
    char *SO_MIN_HOLD_NSEC_str;
    SO_MIN_HOLD_NSEC_str = getenv("SO_MIN_HOLD_NSEC");
    if (SO_MIN_HOLD_NSEC_str == NULL)
        error("environment variable SO_MIN_HOLD_NSEC not set");
    environment->SO_MIN_HOLD_NSEC = atoi(SO_MIN_HOLD_NSEC_str);
#ifdef DEBUG
    printf("environment working");
#endif
}

#endif