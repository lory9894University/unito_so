#define _GNU_SOURCE

#include <stdio.h>
#include "env_operation.h"

void envReading(char **envp, env *environment) {
    char *errorString;
    char *SO_NUM_G_str;
    char *SO_NUM_P_str;
    char *SO_MAX_TIME_str;
    char *SO_BASE_str;
    char *SO_ALTEZZA_str;
    char *SO_FLAG_MIN_str;
    char *SO_FLAG_MAX_str;
    char *SO_ROUND_SCORE_str;
    char *SO_N_MOVES_str;
    char *SO_MIN_HOLD_NSEC_str;


    SO_NUM_G_str = getenv("SO_NUM_G");
    if (SO_NUM_G_str == NULL) {
        errorString = "environment variable SO_NUM_G not set";
        GEN_ERROR
    }
    environment->SO_NUM_G = atoi(SO_NUM_G_str);
    SO_NUM_P_str = getenv("SO_NUM_P");
    if (SO_NUM_P_str == NULL) {
        errorString = "environment variable SO_NUM_P not set";
        GEN_ERROR
    }
    environment->SO_NUM_P = atoi(SO_NUM_P_str);
    SO_MAX_TIME_str = getenv("SO_MAX_TIME");
    if (SO_MAX_TIME_str == NULL) {
        errorString = "environment variable SO_MAX_TIME not set";
        GEN_ERROR
    }
    environment->SO_MAX_TIME = atoi(SO_MAX_TIME_str);
    SO_BASE_str = getenv("SO_BASE");
    if (SO_BASE_str == NULL) {
        errorString = "environment variable SO_BASE not set";
        GEN_ERROR
    }
    environment->SO_BASE = atoi(SO_BASE_str);
    SO_ALTEZZA_str = getenv("SO_ALTEZZA");
    if (SO_ALTEZZA_str == NULL) {
        errorString = "environment variable SO_ALTEZZA not set";
        GEN_ERROR
    }
    environment->SO_ALTEZZA = atoi(SO_ALTEZZA_str);
    SO_FLAG_MIN_str = getenv("SO_FLAG_MIN");
    if (SO_FLAG_MIN_str == NULL) {
        errorString = "environment variable SO_FLAG_MIN not set";
        GEN_ERROR
    }
    environment->SO_FLAG_MIN = atoi(SO_FLAG_MIN_str);
    SO_FLAG_MAX_str = getenv("SO_FLAG_MAX");
    if (SO_FLAG_MAX_str == NULL) {
        errorString = "environment variable SO_FLAG_MAX not set";
        GEN_ERROR
    }
    environment->SO_FLAG_MAX = atoi(SO_FLAG_MAX_str);
    SO_ROUND_SCORE_str = getenv("SO_ROUND_SCORE");
    if (SO_ROUND_SCORE_str == NULL) {
        errorString = "environment variable SO_ROUND_SCORE not set";
        GEN_ERROR
    }
    environment->SO_ROUND_SCORE = atoi(SO_ROUND_SCORE_str);
    SO_N_MOVES_str = getenv("SO_N_MOVES");
    if (SO_N_MOVES_str == NULL) {
        errorString = "environment variable SO_N_MOVES not set";
        GEN_ERROR
    }
    environment->SO_N_MOVES = atoi(SO_N_MOVES_str);
    SO_MIN_HOLD_NSEC_str = getenv("SO_MIN_HOLD_NSEC");
    if (SO_MIN_HOLD_NSEC_str == NULL) {
        errorString = "environment variable SO_MIN_HOLD_NSEC not set";
        GEN_ERROR
    }
    environment->SO_MIN_HOLD_NSEC = atoi(SO_MIN_HOLD_NSEC_str);
#ifdef DEBUG
    printf("environment working");
#endif
}