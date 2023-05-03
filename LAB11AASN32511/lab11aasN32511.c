#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug_mode();
void options(int, char*, char*);
void walk_dir(int, char*, char*);
void error_message(int);

int main(int argc, char *argv[]) {
    int debug = debug_mode();
    switch (argc) {
        case 2:
            options(debug, argv[0], argv[1]);
            break;
        case 3:
            walk_dir(debug, argv[1], argv[2]);
            break;
        default:
            error_message(debug);
            break;
    }
    return EXIT_SUCCESS;
}