#include <stdio.h>
#include "walk_dir.c"
#include "options.c"
#include "LAB11DEBUG.c"

int main(int argc, char *argv[]) {
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        print_version();
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help(argv[0]);
        exit(EXIT_SUCCESS);
    }
    else if (argc==3) {
        walk_dir(argv[1], argv[2]);
    }
    else {
        printf("Your request is invalid. Use command \'-h\' to display help on options.\n");
    }
    return EXIT_SUCCESS;
}