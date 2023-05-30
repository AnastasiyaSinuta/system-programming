#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug_mode();
char* convertToDecimal(char*);
void walk_dir(int, char*, char*);

int main(int argc, char *argv[]) {
    int debug = debug_mode();
    if (argc != 3) { 
        printf("Using: %s <dir> <target>\n", argv[0]); 
        return EXIT_FAILURE; 
    }
    else {
        char* decimal = convertToDecimal(argv[2]);
        if (debug) {
            printf("The target %s in decimal notation: %s\n", argv[2], decimal); 
        }
        walk_dir(debug, argv[1], decimal);
    }
    
    return EXIT_SUCCESS;
}
