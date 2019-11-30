#ifndef SO_PROJECT_ERROR_HANDLING_H
#define SO_PROJECT_ERROR_HANDLING_H

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void error(char *errorString);

void test_error();


#endif //SO_PROJECT_ERROR_HANDLING

