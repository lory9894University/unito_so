#ifndef SO_PROJECT_ERROR_HANDLING_H
#define SO_PROJECT_ERROR_HANDLING_H
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define TEST_ERROR if (errno) {dprintf(STDERR_FILENO,        \
                      "%s:%d: PID=%5d: Error %d (%s)\n", \
                      __FILE__,            \
                      __LINE__,            \
                      getpid(),            \
                      errno,            \
                      strerror(errno));}
//macro ricavata dalle dispense del prof. Bini, mi sembra corretto specificarlo //TODO: lo lascio sto commento?

#define GEN_ERROR printf("error: %s", errorString); exit(1);

#endif //SO_PROJECT_ERROR_HANDLING

