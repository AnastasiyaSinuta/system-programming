#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "libaasN32511.h"

int debug_mode();
void walk_dir(int, char*, char*);
//int check(int, char*, char*);
/*
static struct plugin_option options[] = {
    {
        {
            "bit-seq",
            required_argument,
            0, 0,
        },
        "Search file"
    },
};
static int options_len = sizeof(options)/sizeof(options[0]);

int plugin_get_info(struct plugin_info* ppi) {
    if (!ppi) {
        fprintf(stderr, "ERROR: invalid argument\n");
        return -1;
    }

    ppi->plugin_purpose = "Поиск файлов, содержащих заданную последовательность битов";
    ppi->plugin_author = "Синюта Анастасия Анатольевна, N32511";
    ppi->sup_opts_len = options_len;
    ppi->sup_opts = options;

    return 0;
}
*/
int main(int argc, char *argv[]) {
    int debug = debug_mode();
    /*struct plugin_info *lib;
    if (!plugin_get_info(lib)) {
        printf("Success");
    }
    else {
        printf("Sorry");
    }*/
    if (argc != 3) { 
        printf("Using: %s <dir> <target>\n", argv[0]); 
        return EXIT_FAILURE; 
    }
    walk_dir(debug, argv[1], argv[2]);
    return EXIT_SUCCESS;
}
