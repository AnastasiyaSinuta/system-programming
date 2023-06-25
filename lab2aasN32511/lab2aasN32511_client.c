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
#define MAX_DELAY		2

void	random_delay(int);

int main(int argc, char *argv[]) {
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
            fprintf(stderr, "-a ip (LAB2ADDR) Listening address server and to which client connects.\n");
            fprintf(stderr, "-p port (LAB2PORT) Port on which it listens server and to which client connects.\n");
            fprintf(stderr, "-v Displaying the program version.\n");
            fprintf(stderr, "-h Display help for options.\n");
            if (LAB2DEBUG) fprintf(stderr, "LAB2DEBUG: End debugging.\n");
            exit(EXIT_SUCCESS);
        }
    }
    
    int clientSocket;
	char buffer[BUF_SIZE] = {0};
	struct sockaddr_in serverAddr = {0};

	srand(time(NULL));
	
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	CHECK_RESULT(clientSocket, "socket");

    serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	CHECK_RESULT(res, "connect");

    while (1) {
        printf("Server: Enter string please!\n");
        printf("Client: ");
        char reply[BUF_SIZE];
        if (!fgets(reply, BUF_SIZE, stdin))
        {
            printf("ERROR fgets\n");
            exit(1);
        }
        res = write(clientSocket, reply, strlen(reply) + 1);
        CHECK_RESULT(res, "write");

        res = read(clientSocket, buffer, BUF_SIZE);
        CHECK_RESULT(res, "read");
        printf("Server: %s\n", buffer);

        random_delay(MAX_DELAY);
    }

    close(clientSocket);
	
	return 0;
}

void random_delay(int delay) {
	sleep(1 + rand() % delay);
}
