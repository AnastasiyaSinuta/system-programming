#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void plugins_dir(int, char*);
void and_operation(int, char*, char*);
void or_operation(int);
void inverse_operation(int);
void print_version(int);
void print_help(int, const char*);

void options(int debug, char *argv[], int i) {
    char* option = (char*)argv[i];
    if (strcmp(option, "-P") == 0) {
        plugins_dir(debug, argv[i+1]);
    }
    else if (strcmp(option, "-A") == 0) {
        and_operation(debug, argv[i+1], argv[i+2]);
    }
    else if (strcmp(option, "-O") == 0) {
        or_operation(debug);
    }
    else if (strcmp(option, "-N") == 0) {
        inverse_operation(debug);
    }
    else if (strcmp(option, "-v") == 0) {
        print_version(debug);
    }
    else if (strcmp(option, "-h") == 0) {
        print_help(debug, argv[0]);
    }
    else return;
    if (debug) fprintf(stderr, "debug: End debugging.\n");
    exit(EXIT_SUCCESS);
}

void plugins_dir(int debug, char* dir) {
    if (debug) fprintf(stderr, "debug: Search plugins in %s.\n", dir);

    printf("Еще не сделано\n");
}

void and_operation(int debug, char* first, char* second) {
    if (debug) fprintf(stderr, "debug: Combine %s and %s options.\n", first, second);
    printf("Еще не сделано\n");
}

void or_operation(int debug) {
    if (debug) fprintf(stderr, "debug: \"O\" option.\n");
    printf("Еще не сделано\n");
}

void inverse_operation(int debug) {
    if (debug) fprintf(stderr, "debug: \"N\" option.\n");
    printf("Еще не сделано\n");
}

void print_version(int debug) {
    if (debug) fprintf(stderr, "debug: \"Version\" option.\n");
    char *Version = "1.0";
    const char *Autor = "Sinuta Anastasiya Anatolevna";
    const char *Group = "N32511";
    const int Variant = 5;
    printf("Version: %s\nAutor: %s\nGroup: %s\nVariant: %d\n", Version, Autor, Group, Variant);
}

void print_help(int debug, const char* name) {
    if (debug) fprintf(stderr, "debug: \"Help\" option.\n");
    printf("Usage: %s [options] [directory] [target]...\n", name);
    printf("\nOptions:\n");
    const char* options[] = {
        "\t-P <dir> Directory with plugins..\n",
        "\t-A Combining Plugin Options with an AND Operation (valid by default).\n",
        "\t-O Combining plugin options using the OR operation.\n",
        "\t-N Invert search condition (after merging options plugins with -A or -O).\n",
        "\t-v Displaying the version of the program and information about the program (full name performer, group number, laboratory version number).\n",
        "\t-h Display help for options.\n"
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
