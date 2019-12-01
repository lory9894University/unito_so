#ifndef SO_PROJECT_SHARED_RES_H
#define SO_PROJECT_SHARED_RES_H

typedef struct {
    int base, height;
    char **matrix;
    int *semMatrix;
} table;

typedef struct {
    int value;
    int xPos, yPos;
    int taken;
} flag;

#endif //SO_PROJECT_SHARED_RES_H