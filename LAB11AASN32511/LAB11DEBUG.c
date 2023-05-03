#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_BUFSIZE 80

int debug_mode() {
    char debug_var[DEBUG_BUFSIZE];
    char *envvar = "LAB11DEBUG";
    int debug = 0;

    if (getenv(envvar)) {
        if (snprintf(debug_var, DEBUG_BUFSIZE, "%s", getenv(envvar)) >= DEBUG_BUFSIZE) {
            fprintf(stderr, "debug: DEBUG_BUFSIZE == %d is not enough. Application terminated with an error.\n", DEBUG_BUFSIZE);
            exit(EXIT_FAILURE);
        }

        if (strcmp(debug_var, "1") == 0) {
            debug = 1;
            fprintf(stderr, "debug: Debug mode enabled.\n");
        }
        
        else {
            fprintf(stderr, "debug: Debug mode not enabled.\n");
        }
    }
    return debug;
}