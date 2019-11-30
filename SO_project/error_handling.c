#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void error(char *errorString) {
    printf("error: %s", errorString);
    exit(1);
}

void test_error() {
    if (errno) {
        dprintf(STDERR_FILENO, "%s:%d: PID=%5d: Error %d (%s)\n", __FILE__, __LINE__, getpid(), errno, strerror(errno));
    }
} //funzione scritta prof. Bini, mi sembra corretto specificarlo //TODO: lo lascio sto commento?
