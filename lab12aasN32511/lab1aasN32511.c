#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug_mode();
void options(int, char*[], int);
char* convertToDecimal(char*);
void walk_dir(int, char*, char*);

int main(int argc, char *argv[]) {
    int debug = debug_mode();
    if (argc < 2) {
        fprintf(stderr, "Using: %s <dir> <target>\n", argv[0]);
        return EXIT_FAILURE;
    }
    for (int i = 1; i < argc; i++) {
        options(debug, argv, i);
    }
    char* decimal = convertToDecimal(argv[2]);
    if (debug) fprintf(stderr, "The target %s in decimal notation: %s\n", argv[2], decimal);
    walk_dir(debug, argv[1], decimal);
    return EXIT_SUCCESS;
}
