#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_version(int);
void print_help(int, const char*);
void plugins_dir();
void and_operation();
void or_operation();
void inverse_ooperation();

void options(int debug, char* name, char* option) {
    if (strcmp(option, "-v") == 0) {
        print_version(debug);
    }
    else if (strcmp(option, "-h") == 0) {
        print_help(debug, name);
    }
    else if (strcmp(option, "-P") == 0) {
        plugins_dir();
    }
    else if (strcmp(option, "-A") == 0) {
        and_operation();
    }
    else if (strcmp(option, "-O") == 0) {
        or_operation();
    }
    else if (strcmp(option, "-N") == 0) {
        inverse_operation();
    }
    else return;
    if (debug) fprintf(stderr, "debug: End debugging.\n");
    exit(EXIT_SUCCESS);
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
        "\t-h Print this message and exit.\n",
        "\t-v Print the version, autor, group, variant and exit.\n",
        "\t-P <dir>.\n",
        "\t-A <dir>.\n",
        "\t-O <dir>.\n"
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

void plugins_dir() {
    printf("Еще не сделано\n");
}

void and_operation() {
    printf("Еще не сделано\n");
}

void or_operation() {
    printf("Еще не сделано\n");
}

void inverse_operation() {
    printf("Еще не сделано\n");
}
