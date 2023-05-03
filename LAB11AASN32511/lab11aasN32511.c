#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug_mode();
void options(int, char*);
void walk_dir(int, char*, char*);
void usage_message(int);

int main(int argc, char *argv[]) {
    int debug = debug_mode();
    switch (argc) {
        case 2:
            options(debug, argv[1]);
        case 3:
            walk_dir(debug, argv[1], argv[2]);
        default:
            usage_message(debug);
    }
    return EXIT_SUCCESS;
}