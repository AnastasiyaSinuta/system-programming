#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define CHECK_RESULT(res, msg)  \
    do                          \
    {                           \
        if (res < 0)            \
        {                       \
            perror(msg);        \
            exit(EXIT_FAILURE); \
        }                       \
    } while (0)

#define BUF_SIZE 1024

int convertValueComponent(char *);
char *convertColorSpace(char *);

int main(int argc, char *argv[])
{
    // char* LAB2WAIT = getenv("LAB2WAIT");
    // char* LAB2LOGFILE = getenv("LAB2LOGFILE");
    // char* LAB2ADDR = getenv("LAB2ADDR");
    // char* LAB2PORT = getenv("LAB2PORT");
    char *LAB2DEBUG = getenv("LAB2DEBUG");
    if (LAB2DEBUG)
        fprintf(stderr, "LAB2DEBUG: Debug mode enabled.\n");

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-v"))
        {
            if (LAB2DEBUG)
                fprintf(stderr, "LAB2DEBUG: --Version option called.\n");
            fprintf(stderr, "Lab №2 - \"Processes, threads, and interprocess communication\"\n");
            fprintf(stderr, "Version: 1.0\n");
            fprintf(stderr, "Autor: Sinuta Anastasiya Anatolevna\n");
            fprintf(stderr, "Group: N32511\n");
            fprintf(stderr, "Variant: 25\n");
            if (LAB2DEBUG)
                fprintf(stderr, "LAB2DEBUG: End debugging.\n");
            exit(EXIT_SUCCESS);
        }
        if (!strcmp(argv[i], "-h"))
        {
            if (LAB2DEBUG)
                fprintf(stderr, "LAB2DEBUG: --Help option called.\n");
            fprintf(stderr, "Usage: %s [options]\n", argv[0]);
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "-w N (LAB2WAIT) Imitation of work by suspension 0 Server serving the request process/thread per N seconds. If the option is not given, serve the request without delay (Default: 0).\n");
            fprintf(stderr, "-d Working in daemon mode.\n");
            fprintf(stderr, "-l /path/to/log (LAB2LOGFILE) Path to the log file (Default: /tmp/lab2.log).\n");
            fprintf(stderr, "-a ip (LAB2ADDR) Listening address server and to which client connects.\n");
            fprintf(stderr, "-p port (LAB2PORT) Port on which it listens server and to which client connects.\n");
            fprintf(stderr, "-v Displaying the program version.\n");
            fprintf(stderr, "-h Display help for options.\n");
            if (LAB2DEBUG)
                fprintf(stderr, "LAB2DEBUG: End debugging.\n");
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
    serverAddr.sin_addr.s_addr = INADDR_ANY; // inet_addr("127.0.0.1");

    res = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    CHECK_RESULT(res, "bind");

    res = listen(serverSocket, 5);
    CHECK_RESULT(res, "listen");

    addr_size = sizeof(serverStorage);
    clientSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
    CHECK_RESULT(clientSocket, "accept");

    while (1)
    {
        res = read(clientSocket, buffer, sizeof(buffer));
        CHECK_RESULT(res, "read");

        char *reply = convertColorSpace(buffer);
        res = write(clientSocket, reply, strlen(reply) + 1);
        CHECK_RESULT(res, "write");

        fputs(".", stdout);
        fflush(stdout);
    }

    fputs("\n", stdout);
    close(clientSocket);
    close(serverSocket);

    return 0;
}

char *convertColorSpace(char *request) {
    char *result = NULL;

    // Копируем запрос во временный буфер для обработки
    char *buffer = strdup(request);
    if (!buffer)
    {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    // Разбиваем запрос на компоненты
    char *space = strtok(buffer, " ");
    char *value1 = strtok(NULL, " ");
    char *value2 = strtok(NULL, " ");
    char *value3 = strtok(NULL, "\n");

    // Проверяем наличие всех компонент и их валидность
    if (space == NULL || value1 == NULL || value2 == NULL || value3 == NULL)
    {
        result = strdup("ERROR 1"); // Ошибка: недостаточно компонент
        free(buffer);
        return result;
    }
    // Преобразовываем компоненты в числа
    int comp1 = convertValueComponent(value1);
    int comp2 = convertValueComponent(value2);
    int comp3 = convertValueComponent(value3);
    if (comp1 == -1 || comp2 == -1 || comp3 == -1)
    {
        result = strdup("ERROR 2"); // Ошибка: недопустимый символ в строке
        free(buffer);
        return result;
    }
    // Выполняем преобразование цветовых пространств
    if (!strcmp(space, "RGB"))
    {
        double r = comp1;
        double g = comp2;
        double b = comp3;

        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
        {
            result = strdup("ERROR 4"); // Ошибка: недопустимое значение цвета
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
        if (delta == 0)
        {
            h = 0;
        } // Hue is undefined, so we set it to 0
        else if (maxColor == r)
        {
            h = 60 * ((g - b) / delta);
        }
        else if (maxColor == g)
        {
            h = 60 * ((b - r) / delta + 2);
        }
        else if (maxColor == b)
        {
            h = 60 * ((r - g) / delta + 4);
        }
        // Make sure hue is within the range [0, 360)
        if (h < 0)
            h += 360;

        // Calculate the lightness
        l = (maxColor + minColor) / 2;

        // Calculate the saturation
        if (delta == 0)
        {
            s = 0;
        }
        else
        {
            s = delta / (1 - fabs(2 * l - 1));
        }

        char answer[BUF_SIZE];
        sprintf(answer, "HSL %.2f %.2f %.2f", h, s, l);
        result = strdup(answer);
    }
    else if (!strcmp(space, "HSL"))
    {
        double h = comp1;
        double s = comp2 / 100.0;
        double l = comp3 / 100.0;

        if (h < 0 || h > 360 || s < 0 || s > 1 || l < 0 || l > 1)
        {
            result = strdup("ERROR 4"); // Ошибка: недопустимое значение цвета
            free(buffer);
            return result;
        }

        double c = (1 - fabs(2 * l - 1)) * s;
        double x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
        double m = l - c / 2.0;

        double r, g, b;

        if (h >= 0 && h < 60)
        {
            r = c;
            g = x;
            b = 0;
        }
        else if (h >= 60 && h < 120)
        {
            r = x;
            g = c;
            b = 0;
        }
        else if (h >= 120 && h < 180)
        {
            r = 0;
            g = c;
            b = x;
        }
        else if (h >= 180 && h < 240)
        {
            r = 0;
            g = x;
            b = c;
        }
        else if (h >= 240 && h < 300)
        {
            r = x;
            g = 0;
            b = c;
        }
        else
        {
            r = c;
            g = 0;
            b = x;
        }

        int red = (int)((r + m) * 255);
        int green = (int)((g + m) * 255);
        int blue = (int)((b + m) * 255);

        char *answer = malloc(BUF_SIZE);
        sprintf(answer, "RGB %d %d %d", red, green, blue);
        result = strdup(answer);
    }
    else
    {
        result = strdup("ERROR 3"); // Ошибка: неподдерживаемое цветовое пространство
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
