#include <stdio.h>
#include <stdlib.h>

#define DEBUG_BUFSIZE 80

int debug_mode() {
    char debug_var[DEBUG_BUFSIZE];
    char *envvar = "LAB11DEBUG";
    int debug = 0;

    if (getenv(envvar)) {
        if (snprintf(debug_var, DEBUG_BUFSIZE, "%s", getenv(envvar)) >= DEBUG_BUFSIZE) {
            fprintf(stderr, "DEBUG_BUFSIZE == %d is too small. Application terminated.\n", DEBUG_BUFSIZE);
            exit(EXIT_FAILURE);
        }

        if (strcmp(debug_var, "1") == 0) {
            debug = 1;
            fprintf(stderr, "DEBUG: Application will be executed in Debug mode.\n");
        }
        
        else {
            fprintf(stderr, "DEBUG: Debug variable found, but it is not equal to 1. Application will NOT be executed in Debug mode.\n");
        }
    }
    return debug;
}