#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>

void playerBirth(int pawnNumber) {
    printf("prova");
    execve("sl", NULL, NULL);
}

