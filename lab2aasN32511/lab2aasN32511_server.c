#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    //char* LAB2WAIT = getenv("LAB2WAIT");
    //char* LAB2LOGFILE = getenv("LAB2LOGFILE");
    //char* LAB2ADDR = getenv("LAB2ADDR");
    //char* LAB2PORT = getenv("LAB2PORT");
    char* LAB2DEBUG = getenv("LAB2DEBUG");
    if (LAB2DEBUG) fprintf(stderr, "LAB2DEBUG: Debug mode enabled.\n");

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-v")) {
            if (LAB2DEBUG) fprintf(stderr, "LAB2DEBUG: --Version option called.\n");
            fprintf(stderr, "Lab №2 - \"Processes, threads, and interprocess communication\"\n");
            fprintf(stderr, "Version: 1.0\n");
            fprintf(stderr, "Autor: Sinuta Anastasiya Anatolevna\n");
            fprintf(stderr, "Group: N32511\n");
            fprintf(stderr, "Variant: 25\n");
            if (LAB2DEBUG) fprintf(stderr, "LAB2DEBUG: End debugging.\n");
            exit(EXIT_SUCCESS);
        }
        if (!strcmp(argv[i], "-h")) {
            if (LAB2DEBUG) fprintf(stderr, "LAB2DEBUG: --Help option called.\n");
            fprintf(stderr, "Usage: %s [options]\n", argv[0]);
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "-w N (LAB2WAIT) Imitation of work by suspension 0 Server serving the request process/thread per N seconds. If the option is not given, serve the request without delay (Default: 0).\n");
            fprintf(stderr, "-d Working in daemon mode.\n");
            fprintf(stderr, "-l /path/to/log (LAB2LOGFILE) Path to the log file (Default: /tmp/lab2.log).\n");
            fprintf(stderr, "-a ip (LAB2ADDR) Listening address server and to which client connects.\n");
            fprintf(stderr, "-p port (LAB2PORT) Port on which it listens server and to which client connects.\n");
            fprintf(stderr, "-v Displaying the program version.\n");
            fprintf(stderr, "-h Display help for options.\n");
            if (LAB2DEBUG) fprintf(stderr, "LAB2DEBUG: End debugging.\n");
            exit(EXIT_SUCCESS);
        }
    }

    return EXIT_SUCCESS;
}