#include <stdio.h>
#include <stdlib.h>

// incorrect usage message
void usage_message(int debug) {
    if (debug) fprintf(stderr, "DEBUG: Incorrect usage. Outputting usage and quitting.\n");
    printf("Your request is invalid. Use command \'-h\' to display help on options.\n");
    exit(EXIT_FAILURE);
}