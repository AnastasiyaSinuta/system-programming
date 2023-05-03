#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_version(int);
void print_help(int, const char*);
void usage_message(int);

void options(int debug, const char* argv) {
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        print_version(debug);
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help(debug, argv[0]);
        exit(EXIT_SUCCESS);
    }
    else {  
        usage_message(debug);
    }
}

void print_version(int debug) {
    if (debug) fprintf(stderr, "DEBUG: Outputting \"Help\" option.\n");
    char *Version = "1.0";
    const char *Autor = "Sinuta Anastasiya Anatolevna";
    const char *Group = "N32511";
    const int Variant = 5;
    printf("Version: %s\nAutor: %s\nGroup: %s\nVariant: %d\n", Version, Autor, Group, Variant);
}

void print_help(int debug, const char* name) {
    if (debug) fprintf(stderr, "DEBUG: Outputting \"Version\" option.\n");
    printf("Usage: %s [options] [directory] [target]...\n", name);
    printf("\nOptions:\n");
    const char* options[] = {
        "\t-h, --help Print this message and exit.\n",
        "\t-v, --version Print the version, autor, group, variant and exit.\n"
    };
    int count = sizeof(options)/sizeof(options[0]);
    for (int i = 0; i < count; i++) {
        printf("%s", options[i]);
    }
    printf("\nDirectory:\n");
    printf("\tSpecify the byte sequences to search. The target argument is in the format of an encoded string UTF-8.\n");
    printf("\nTarget:\n");
    printf("\tSpecify the path to the directory, starting from the directory \'/home\', from which you want to start the search.\n");
    printf("\nThis program built for x86_64-pc-linux-gnu\n");
    printf("Report bugs to <336972@niuitmo.ru>\n");
}