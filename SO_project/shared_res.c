#define _GNU_SOURCE

#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error_handling.h"
#include "shared_res.h"

/*semOp, 1 release, -1 reserve*/
int semHandling(int semId, int semNum, int semOp) {
    struct sembuf ops;

    ops.sem_num = semNum;
    ops.sem_op = semOp;
    ops.sem_flg = 0;
    return semop(semId, &ops, 1);
}

int semHandlingTimed(int semId, int semNum, int semOp) {
    struct sembuf ops;
    struct timespec ts;

    ts.tv_sec = 0;
    ts.tv_nsec = 100000000;/*0.1 sec*/
    ops.sem_num = semNum;
    ops.sem_op = semOp;
    ops.sem_flg = 0;
    return semtimedop(semId, &ops, 1, &ts);
}

/*list operation*/
syncNode newNode(syncNode next) {
    syncNode x = malloc(sizeof(struct node));
    x->next = next;
    return x;
}

void freeList(syncNode head) {
    syncNode oldNode;
    if (head == NULL)
        return;
    while (head->next != NULL) {
        oldNode = head;
        free(oldNode);
        head = head->next;
    }
    free(head);
}

void insertList(syncNode *head, int distance, int semIndex) {
    syncNode x, p;
    if (*head == NULL || ((*head)->distance < distance)) {
        x = newNode(*head);
        *head = x;
        x->distance = distance;
        x->semIndex = semIndex;
    } else {
        p = (*head)->next;
        x = *head;
        while (p != NULL && ((p)->distance > distance)) {
            x = p;
            p = p->next;
        }
        x->next = newNode(p);
        x = x->next;
        x->distance = distance;
        x->semIndex = semIndex;
    }
}

void changePriorityList(syncNode *head, int newDistance) {
    syncNode moved, x, p;

    moved = *head;
    moved->distance = newDistance;
    *head = (*head)->next;
    p = (*head)->next;
    x = *head;
    while (p != NULL && (p->distance > newDistance)) {
        x = p;
        p = p->next;
    }
    x->next = moved;
    moved->next = p;
}
