#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_version();
void print_help(char*);
void walk_dir(char*, char*);

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        printf("Your request is invalid. Use command \'-h\' to display help on options.\n");
        exit(EXIT_FAILURE);
    }
    else if (argc == 2){
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
            print_version();
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_help(argv[0]);
            exit(EXIT_SUCCESS);
        }
        else {
            printf("Your request is invalid. Use command \'-h\' to display help on options.\n");
            exit(EXIT_FAILURE);
        }
    }
    else{
        walk_dir(argv[1], argv[2]);
    }
    return EXIT_SUCCESS;
}