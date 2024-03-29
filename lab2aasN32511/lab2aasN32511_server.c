#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <termios.h>

#define BUF_SIZE 1024
char *LAB2DEBUG;
FILE * logfile;
int delay;
int serverSocket;
int success_requests = 0;
int error_requests = 0;
int all_requests = 0;
time_t start_time;
int convertValueComponent(char*);
char *convertColorSpace(char*);

void writeLog(FILE *file, const char *format, ...) {
    time_t rawtime;
    struct tm *timeinfo;
    char timestamp[20];

    // Получение текущего времени
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%d.%m.%y %H:%M:%S", timeinfo);

    // Запись данных в файл с временной меткой и значениями
    fprintf(file, "[%s] ", timestamp);

    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);

    fprintf(file, "\n");
    fflush(file);
}

void CHECK_RESULT(int res, char *msg) {
    if (res < 0) {
        writeLog(logfile, "ERROR: %s: %s.", msg, strerror(errno));
        if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
        exit(EXIT_FAILURE);
    }
}

void handle_signal(int sig) {
    switch (sig) {
    case SIGINT:
        writeLog(logfile, "Received SIGINT. Terminating...");
        break;
    case SIGTERM:
        writeLog(logfile, "Received SIGTERM. Terminating...");
        break;
    case SIGQUIT:
        writeLog(logfile, "Received SIGQUIT. Terminating...");
        break;
    case SIGUSR1:
        time_t current_time = time(NULL);
        double uptime = difftime(current_time, start_time);
        writeLog(logfile, "Received SIGUSR1. Statistics:");
        writeLog(logfile, "Uptime: %.2f seconds", uptime);
        writeLog(logfile, "Successful requests: %d", success_requests);
        writeLog(logfile, "Error requests: %d", error_requests);
        break;
    }
    if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
    fclose(logfile);
    close(serverSocket);
    exit(EXIT_SUCCESS);
}

void* client_handler(void* arg) {
    int current_request = ++all_requests;
    writeLog(logfile, "Start request %d", current_request);
    int clientSocket = *(int *)arg;
    char buffer[BUF_SIZE];

    // Чтение данных из клиентского сокета
    memset(buffer, 0, sizeof(buffer));
    ssize_t n = read(clientSocket, buffer, sizeof(buffer) - 1);
    if (n < 0) {
        error_requests++;
        writeLog(logfile, "ERROR: read: %s.", strerror(errno));
        goto end;
    }
    writeLog(logfile, "Client: %s", buffer);

    sleep(delay);
    
    // Обработка клиентского запроса
    char *reply = convertColorSpace(buffer);
    if (strstr(reply, "ERROR")) { error_requests++; }
    else success_requests++;

    // Отправка данных клиенту
    writeLog(logfile, "Server: %s", reply);
    if (write(clientSocket, reply, n) < 0) {
        error_requests++;
        writeLog(logfile, "ERROR: write: %s.", strerror(errno));
        goto end;
    }
    end:
    // Закрытие клиентского сокета
    writeLog(logfile, "End request %d", current_request);
    close(clientSocket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    start_time = time(NULL);
    char *pathToLog = "./tmp/lab2.log";
    char *LAB2LOGFILE = getenv("LAB2LOGFILE");
    if (LAB2LOGFILE) pathToLog = LAB2LOGFILE;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-l")) {
            if (i == argc-1) {
                fprintf(stderr, "ERROR: The -l option needs an argument.\n");
                exit(EXIT_FAILURE);
            } else {
                pathToLog = argv[i+1];
                break;
            }
        }
    }
    logfile = fopen(pathToLog, "w");
    if (logfile == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    LAB2DEBUG = getenv("LAB2DEBUG");
    if (LAB2DEBUG) writeLog(logfile, "DEBUG: Start debugging.");
    if (getenv("LAB2WAIT")) delay = atoi(getenv("LAB2WAIT"));
    char *address_ip = getenv("LAB2ADDR");
    int port = 0;
    if (getenv("LAB2PORT")) port = atoi(getenv("LAB2PORT"));
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v")) {
            if (LAB2DEBUG) writeLog(logfile, "DEBUG: --Version option called.");
            writeLog(logfile, "Lab №2 - \"Processes, threads, and interprocess communication\"");
            writeLog(logfile, "Version: 1.0");
            writeLog(logfile, "Autor: Sinuta Anastasiya Anatolevna");
            writeLog(logfile, "Group: N32511");
            writeLog(logfile, "Variant: 25");
            if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
            exit(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-h")) {
            if (LAB2DEBUG) writeLog(logfile, "DEBUG: --Help option called.");
            writeLog(logfile, "Usage: %s [options]", argv[0]);
            writeLog(logfile, "Options:");
            writeLog(logfile, "-w N (LAB2WAIT) Imitation of work by suspension serving the request process/thread per N seconds. If the option is not given, serve the request without delay (Default: 0).");
            writeLog(logfile, "-d Working in daemon mode.");
            writeLog(logfile, "-l /path/to/log (LAB2LOGFILE) Path to the log file (Default: /tmp/lab2.log).");
            writeLog(logfile, "-a ip (LAB2ADDR) Listening address server and to which client connects.");
            writeLog(logfile, "-p port (LAB2PORT) Port on which it listens server and to which client connects.");
            writeLog(logfile, "-v Displaying the program version.");
            writeLog(logfile, "-h Display help for options.");
            if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
            exit(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-w")) {
            if (i != argc - 1) {
                delay = atoi(argv[i+1]);
                i++;
            } else {
                writeLog(logfile, "ERROR: The -w option needs an argument.");
                if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
                exit(EXIT_FAILURE);
            }
        } else if (!strcmp(argv[i], "-d")) {
            if (LAB2DEBUG) writeLog(logfile, "DEBUG: Server part working in daemon mode.");
            pid_t pid, sid;
            // Создание дочернего процесса
            pid = fork();
            // Если не удалось создать дочерний процесс, завершаем программу
            if (pid < 0) {
                writeLog(logfile, "ERROR: fork.");
                if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
                exit(EXIT_FAILURE);
            }
            // Если процесс создан успешно, завершаем родительский процесс
            if (pid > 0) {
                exit(EXIT_SUCCESS);
            }
            // Изменение файловой маски для создания файлов с нужными правами
            umask(0);
            // Создание новой сессии
            sid = setsid();
            if (sid < 0) {
                writeLog(logfile, "ERROR: setsid.");
                if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
                exit(EXIT_FAILURE);
            }
            // Закрытие стандартных файловых дескрипторов
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
        } else if (!strcmp(argv[i], "-l")) {
            i++;
            break; 
        } 
        else if (!strcmp(argv[i], "-a")) {
            if (i != argc-1) {
                address_ip = strdup(argv[i+1]);
                i++;
            } else {
                writeLog(logfile, "ERROR: The -a option needs an argument.");
                if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
                exit(EXIT_FAILURE);
            }
        } else if (!strcmp(argv[i], "-p")) {
            if (i != argc-1) {
                port = atoi(argv[i+1]);
                i++;
            } else {
                writeLog(logfile, "ERROR: The -p option needs an argument.");
                if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
                exit(EXIT_FAILURE);
            }
        } else {
            writeLog(logfile, "ERROR: Unknown option %s", argv[i]);
            if (LAB2DEBUG) writeLog(logfile, "DEBUG: End debugging.");
            exit(EXIT_FAILURE);
        }
    }
    if (LAB2DEBUG) writeLog(logfile, "DEBUG: Delay for %d second included.", delay);
    
    int clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t client_len;
    pthread_t tid;
    int res;

    // Создание сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_RESULT(serverSocket, "socket");
    if (LAB2DEBUG) writeLog(logfile, "DEBUG: Successful socket.");
    
    // Настройка серверного адреса
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // IPv4
    if (address_ip != NULL) { serverAddr.sin_addr.s_addr = inet_addr(address_ip); }
    else serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (LAB2DEBUG) writeLog(logfile, "DEBUG: Listening address server: %s", inet_ntoa(serverAddr.sin_addr));
    if (port != 0) { serverAddr.sin_port = htons(port); }
    else serverAddr.sin_port = htons(5555);
    if (LAB2DEBUG) writeLog(logfile, "DEBUG: Port on which it listens server: %d", serverAddr.sin_port);

    // Привязка сокета к серверному адресу
    res = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    CHECK_RESULT(res, "bind");
    if (LAB2DEBUG) writeLog(logfile, "DEBUG: Successful bind.");
    
    // Ожидание входящих соединений
    res = listen(serverSocket, 5);
    CHECK_RESULT(res, "listen");
    if (LAB2DEBUG) writeLog(logfile, "DEBUG: Successful listen.");

    // Установка обработчика сигналов
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGQUIT, handle_signal);
    signal(SIGUSR1, handle_signal);

    while (1) {
        // Принятие входящего соединения
        client_len = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &client_len);
        CHECK_RESULT(clientSocket, "accept");
        if (LAB2DEBUG) writeLog(logfile, "DEBUG: Successful accept.");

        // Создание потока для обработки клиентского соединения
        if (pthread_create(&tid, NULL, client_handler, &clientSocket) != 0) { 
            writeLog(logfile, "ERROR: pthread_create: %s.", strerror(errno));
            exit(EXIT_FAILURE);
        }
        pthread_detach(tid);
    }
    fclose(logfile);
    close(serverSocket);
    exit(EXIT_SUCCESS);
}

char *convertColorSpace(char *request) {
    char *result = NULL;

    // Копируем запрос во временный буфер для обработки
    char *buffer = strdup(request);
    if (!buffer) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    // Разбиваем запрос на компоненты
    char *space = strtok(buffer, " ");
    char *value1 = strtok(NULL, " ");
    char *value2 = strtok(NULL, " ");
    char *value3 = strtok(NULL, "\n");

    // Проверяем наличие всех компонент и их валидность
    if (space == NULL || value1 == NULL || value2 == NULL || value3 == NULL) {
        result = "ERROR 1"; // Ошибка: недостаточно компонент
        free(buffer);
        return result;
    }
    // Преобразовываем компоненты в числа
    int comp1 = convertValueComponent(value1);
    int comp2 = convertValueComponent(value2);
    int comp3 = convertValueComponent(value3);
    if (comp1 == -1 || comp2 == -1 || comp3 == -1) {
        result = "ERROR 2"; // Ошибка: недопустимый символ в строке
        free(buffer);
        return result;
    }
    // Выполняем преобразование цветовых пространств
    if (!strcmp(space, "RGB") || !strcmp(space, "rgb")) {
        double r = comp1;
        double g = comp2;
        double b = comp3;

        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            result = "ERROR 4"; // Ошибка: недопустимое значение цвета
            free(buffer);
            return result;
        }

        r /= 255.0;
        g /= 255.0;
        b /= 255.0;

        double maxColor = fmax(r, fmax(g, b));
        double minColor = fmin(r, fmin(g, b));
        double delta = maxColor - minColor;
        double h = 0, s = 0, l = 0;

        // Calculate the hue
        if (delta == 0) { h = 0; } // Hue is undefined, so we set it to 0
        else if (maxColor == r) { h = 60 * ((g - b) / delta); }
        else if (maxColor == g) { h = 60 * ((b - r) / delta + 2); }
        else if (maxColor == b) { h = 60 * ((r - g) / delta + 4); }
        // Make sure hue is within the range [0, 360)
        if (h < 0) h += 360;

        // Calculate the lightness
        l = (maxColor + minColor) / 2;

        // Calculate the saturation
        if (delta == 0) { s = 0; }
        else s = delta / (1 - fabs(2 * l - 1));

        char* answer = malloc(BUF_SIZE * sizeof(char));
        sprintf(answer, "HSL %.2f %.2f %.2f", h, s, l);
        result = answer;
    } else if (!strcmp(space, "HSL") || !strcmp(space, "hsl")) {
        double h = comp1;
        double s = comp2 / 100.0;
        double l = comp3 / 100.0;

        if (h < 0 || h > 360 || s < 0 || s > 1 || l < 0 || l > 1) {
            result = "ERROR 4"; // Ошибка: недопустимое значение цвета
            free(buffer);
            return result;
        }

        double c = (1 - fabs(2 * l - 1)) * s;
        double x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
        double m = l - c / 2.0;

        double r, g, b;

        if (h >= 0 && h < 60) {
            r = c; g = x; b = 0;
        } else if (h >= 60 && h < 120) {
            r = x; g = c; b = 0;
        } else if (h >= 120 && h < 180) {
            r = 0; g = c; b = x;
        } else if (h >= 180 && h < 240) {
            r = 0; g = x; b = c;
        } else if (h >= 240 && h < 300) {
            r = x; g = 0; b = c;
        } else {
            r = c; g = 0; b = x;
        }

        int red = (int)((r + m) * 255);
        int green = (int)((g + m) * 255);
        int blue = (int)((b + m) * 255);

        char* answer = malloc(BUF_SIZE * sizeof(char));
        sprintf(answer, "RGB %d %d %d", red, green, blue);
        result = answer;
    } else {
        result = "ERROR 3"; // Ошибка: неподдерживаемое цветовое пространство
        free(buffer);
        return result;
    }
    free(buffer);
    return result;
}

int convertValueComponent(char* value) {
    char* buffer = strdup(value);
    if (!buffer) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    int len = strlen(buffer);
    int component = 0;
    if (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X')) {
        for (int i = 2; i < len; i++) {
            char digit = buffer[i];
            int val;
            if (digit >= '0' && digit <= '9') {
                val = digit - '0';
            } else if (digit >= 'A' && digit <= 'F') {
                val = 10 + (digit - 'A');
            } else if (digit >= 'a' && digit <= 'f') {
                val = 10 + (digit - 'a');
            } else {
                free(buffer);
                return -1;
            }
            component += val * pow(16, len - i - 1);
        }
    } else {
        for (int i = 0; i < len; i++) {
            if (buffer[i] < '0' || buffer[i] >'9') {
                free(buffer);
                return -1;
            }
        }
        component = strtol(value, NULL, 0);
    }
    free(buffer);
    return component;
}
