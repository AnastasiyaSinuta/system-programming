#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fts.h>
#include <errno.h>
#include <math.h>
#include "plugin_api.h"

static struct plugin_option arr[] = {
    {
        {"bit-seq",
           1,
           0, 0,
        },
    "Target sequence of bits"
    }
};

int plugin_get_info(struct plugin_info* ppi) {
    if (!ppi) {
        fprintf(stderr, "ERROR: invalid argument\n");
        return -1;
    }
    ppi->plugin_purpose = "Search for files containing a given sequence of bits.";
    ppi->plugin_author = "Sinuta Anastasiya";
    ppi->sup_opts_len = sizeof(arr)/sizeof(arr[0]);
    ppi->sup_opts = arr;

    return 0;
}

int convertToDecimal(const char* number) {
    int base = 0;
    int i = 0;
    int decimal = 0;
    
    if (number[0] == '0' && number[1] == 'b') {
        base = 2;
        i = 2;
    } else if (number[0] == '0' && (number[1] == 'x' || number[1] == 'X')) {
        base = 16;
        i = 2;
    } else {
        base = 10;
        i = 0;
    }
    
    int len = strlen(number);
    for (; i < len; i++) {
        char digit = number[i];
        int value;
        
        if (digit >= '0' && digit <= '9') {
            value = digit - '0';
        } else if (digit >= 'A' && digit <= 'F') {
            value = 10 + (digit - 'A');
        } else if (digit >= 'a' && digit <= 'f') {
            value = 10 + (digit - 'a');
        } else {
            return -1;
        }

        decimal += value * pow(base, len - i - 1);
    }
    return decimal;
}

int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) {
    if (! fname || !in_opts || in_opts_len <= 0){
        errno = EINVAL;
        return -1;
    }

    if (getenv("LAB1DEBUG")) fprintf(stderr, "debug: Found file \"%s\"\n", (char*)fname);
    
    FILE *fp;
    for (size_t i = 0; i < in_opts_len; i++) {
        int decimal = convertToDecimal((char *)in_opts[i].flag);
        if (decimal == -1) {
            fprintf(stderr, "Invalid number format. Non-numeric digit found.\n");
            return -1;
        }
        char buffer[20];
        sprintf(buffer, "%d", decimal);
        char* target = malloc(strlen(buffer) + 1);
        strcpy(target, buffer);
        if (getenv("LAB1DEBUG")) fprintf(stderr, "debug: The target in decimal notation: %s\n", target);
        
        // Открываем файл для чтения
        fp = fopen(fname, "rb");
        if (fp == NULL) {
            fprintf(stderr, "Error opening file %s: %s\n", fname, strerror(errno));
            return -1;
        }
        // Получаем размер файла
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        rewind(fp);
        
        // Выделяем память для хранения содержимого файла
        char *content = malloc(fsize + 1);
        if (content == NULL) {
            fprintf(stderr, "Error allocating memory: %s\n", strerror(errno));
            fclose(fp);
            return -1;
        }

        // Читаем содержимое файла
        if (fread(content, fsize, 1, fp) != 1) {
            fprintf(stderr, "Error reading file %s: %s\n", fname, strerror(errno));
            free(content);
            fclose(fp);
            return -1;
        }

        // Добавляем завершающий нулевой символ
        content[fsize] = '\0';

        // Ищем последовательность байтов в содержимом файла
        char *result = strstr(content, target);
        if (result != NULL) {
            // Вычисляем позицию, где найдена последовательность
            long pos = result - content;
            printf("File %s contains the target sequence of bytes at position %ld\n", fname, pos);
            // Освобождаем выделенную память и закрываем файл
            free(content);
            fclose(fp);
            return 0;
        }
        else {
            if (getenv("LAB1DEBUG")) printf("debug: File %s does not contain the target sequence of bytes\n", fname);
            // Освобождаем выделенную память и закрываем файл
            free(content);
            fclose(fp);
            return 1;
        }
    }
    return 0;
}
