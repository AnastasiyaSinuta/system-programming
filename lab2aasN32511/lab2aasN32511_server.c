#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CHECK_RESULT(res, msg)			\
do {									\
	if (res < 0) {						\
		perror(msg);					\
		exit(EXIT_FAILURE);				\
	}									\
} while (0)

#define BUF_SIZE		1024

char*   coverter(char*);

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
    
    int serverSocket, clientSocket;
	char buffer[BUF_SIZE];
	struct sockaddr_in serverAddr = {0};
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
	int res;
	
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	CHECK_RESULT(serverSocket, "socket");

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");

	res = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	CHECK_RESULT(res, "bind");

	res = listen(serverSocket, 5);
	CHECK_RESULT(res, "listen");

    addr_size = sizeof(serverStorage);
	clientSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
	CHECK_RESULT(clientSocket, "accept");

    while (1) {
        res = read(clientSocket, buffer, sizeof(buffer));
        CHECK_RESULT(res, "read");

        char *reply = coverter(buffer);
        res = write(clientSocket, reply, strlen(reply) + 1);
        CHECK_RESULT(res, "write");

        fputs(".", stdout); fflush(stdout);
    }
    
	fputs("\n", stdout);	
	close(clientSocket);
	close(serverSocket);

	return 0;
}

char* coverter(char *str) {
    return str;
}
