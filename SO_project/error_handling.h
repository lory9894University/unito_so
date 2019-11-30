#include <stdlib.h>
#include <stdio.h>

void error(char *errorString) {
    printf("error: %s", errorString);
    exit(1);
}
