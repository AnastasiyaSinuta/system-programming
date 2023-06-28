#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 1024
char *LAB2DEBUG;
void CHECK_RESULT(int res, char *msg) {
    if (res < 0) {
        fprintf(stderr, "ERROR: %s: %s.\n", msg, strerror(errno));
        if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    char *address_ip = getenv("LAB2ADDR");
    int port = 0;
    if (getenv("LAB2PORT")) port = atoi(getenv("LAB2PORT"));
    LAB2DEBUG = getenv("LAB2DEBUG");
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
            if (i != argc-1) {
                address_ip = argv[i+1];
                i++;
            } else {
                fprintf(stdout, "ERROR: The -a option needs an argument.\n");
                if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
                exit(EXIT_FAILURE);
            }
        } else if (!strcmp(argv[i], "-p")) {
            if (i != argc-1) {
                port = atoi(argv[i+1]);
                i++;
            } else {
                fprintf(stdout, "ERROR: The -p option needs an argument.\n");
                if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "ERROR: Unknown option %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUF_SIZE];

    // Создание сокета
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_RESULT(clientSocket, "socket");
    if (LAB2DEBUG) fprintf(stdout, "DEBUG: Successful socket.\n");
    
    // Настройка серверного адреса
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // IPv4
    if (address_ip != NULL) { serverAddr.sin_addr.s_addr = inet_addr(address_ip); }
    else serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (LAB2DEBUG) fprintf(stdout, "DEBUG: Address to which client connects: %s\n", inet_ntoa(serverAddr.sin_addr));
    if (port != 0) { serverAddr.sin_port = htons(port); }
    else serverAddr.sin_port = htons(5555);
    if (LAB2DEBUG) fprintf(stdout, "DEBUG: Port to which client connects: %d\n", serverAddr.sin_port);
    
    // Инициализация соединения на сокете
    int res = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    CHECK_RESULT(res, "connect");
    if (LAB2DEBUG) fprintf(stdout, "DEBUG: Successful connect.\n");

    // Отправка данных серверу
    printf("Client: ");
    char *reply = NULL;
    size_t reply_size = 0;
    ssize_t length;
    length = getline(&reply, &reply_size, stdin);
    if (length > 0 && reply[length - 1] == '\n') { 
        reply[length - 1] = '\0'; 
        length--; 
    }
    res = write(clientSocket, reply, BUF_SIZE);
    CHECK_RESULT(res, "write");
    free(reply);

    // Чтение данных из серверного сокета 
    res = read(clientSocket, buffer, sizeof(buffer) - 1);
    CHECK_RESULT(res, "read");
    printf("Server: %s\n", buffer);

    // Закрытие клиентского сокета
    close(clientSocket);
    if (LAB2DEBUG) fprintf(stdout, "DEBUG: End debugging.\n");
    exit(EXIT_SUCCESS);
}