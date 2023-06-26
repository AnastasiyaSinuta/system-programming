#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define CHECK_RESULT(res, msg)          \
do {                                    \
    if (res < 0) {                      \
        perror(msg);                    \
        exit(EXIT_FAILURE);             \
    }                                   \
} while (0)

#define BUF_SIZE    1024

int main(int argc, char *argv[]) {
    char *LAB2ADDR = getenv("LAB2ADDR");
    char *address_ip = NULL;
    char *LAB2PORT = getenv("LAB2PORT");
    int port = 0;
    char* LAB2DEBUG = getenv("LAB2DEBUG");
    if (LAB2DEBUG) fprintf(stdout, "DEBUG: Start debugging.\n");

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v")) {
            if (LAB2DEBUG) fprintf(stdin, "DEBUG: --Version option called.\n");
            fprintf(stdout, "Lab №2 - \"Processes, threads, and interprocess communication\"\n");
            fprintf(stdout, "Version: 1.0\n");
            fprintf(stdout, "Autor: Sinuta Anastasiya Anatolevna\n");
            fprintf(stdout, "Group: N32511\n");
            fprintf(stdout, "Variant: 25\n");
            if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
            exit(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-h")) {
            if (LAB2DEBUG) fprintf(stdout, "DEBUG: --Help option called.\n");
            fprintf(stdout, "Usage: %s [options]\n", argv[0]);
            fprintf(stdout, "Options:\n");
            fprintf(stdout, "-a ip (LAB2ADDR) Listening address server and to which client connects.\n");
            fprintf(stdout, "-p port (LAB2PORT) Port on which it listens server and to which client connects.\n");
            fprintf(stdout, "-v Displaying the program version.\n");
            fprintf(stdout, "-h Display help for options.\n");
            if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
            exit(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-a")) {
            if (LAB2ADDR) {
                if (LAB2DEBUG) fprintf(stdout, "DEBUG: LAB2ADDR environment variable enabled.\n");
                if (i != argc-1) {
                    address_ip = argv[i+1];
                    if (LAB2DEBUG) fprintf(stdout, "DEBUG: Address to which client connects: %s.\n", address_ip);
                    i++;
                } else {
                    fprintf(stdout, "ERROR: The -a option needs an argument.\n");
                    if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
                    exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stdout,"ERROR: To work with the -a option, enable the LAB2ADDR environment variable.\n");
                if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
                exit(EXIT_FAILURE);
            }
        } else if (!strcmp(argv[i], "-p")) {
            if (LAB2PORT) {
                if (LAB2DEBUG) fprintf(stdout, "DEBUG: LAB2PORT environment variable enabled.\n");
                if (i != argc-1) {
                    port = atoi(argv[i+1]);
                    if (LAB2DEBUG) fprintf(stdout, "DEBUG: Port to which client connects: %d.\n", port);
                    i++;
                } else {
                    fprintf(stdout, "ERROR: The -p option needs an argument.\n");
                    if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
                    exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stdout,"ERROR: To work with the -p option, enable the LAB2PORT environment variable.\n");
                if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "ERROR: Unknown option %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    
    int clientSocket;
	char buffer[BUF_SIZE] = {0};
	struct sockaddr_in serverAddr = {0};
	
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	CHECK_RESULT(clientSocket, "socket");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (port != 0) {
        serverAddr.sin_port = htons(port);
    }
    if (address_ip != NULL) {
        serverAddr.sin_addr.s_addr = inet_addr(address_ip);
    }

	int res = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	CHECK_RESULT(res, "connect");
	
	printf("Client: ");
    char reply[BUF_SIZE];
    if (fgets(reply, BUF_SIZE, stdin) == NULL) {
        printf("ERROR fgets\n");
        exit(1);
    }
    res = write(clientSocket, reply, strlen(reply) + 1);
    CHECK_RESULT(res, "write");

    res = read(clientSocket, buffer, BUF_SIZE);
    CHECK_RESULT(res, "read");
    printf("Server: %s\n", buffer);

    close(clientSocket);
    if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
    exit(EXIT_SUCCESS);
}