#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
