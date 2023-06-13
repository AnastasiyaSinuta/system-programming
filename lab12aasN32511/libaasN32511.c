#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fts.h>
#include <errno.h>

#include "plugin_api.h"


static char *g_lib_name = "libaasN32511.so";

static char *g_plugin_purpose = "Chek if the file contains a given sequence of bits";

static char *g_plugin_author = "Sinuta Anastasiya";

#define OPT_BIT_SEQ_STR "bit-seq"

static struct plugin_option g_po_arr[] = {
    {
        {
            OPT_BIT_SEQ_STR,
            required_argument,
            0, 0,
        },
    "Target sequence of bits"
    }
};

static int g_po_arr_len = sizeof(g_po_arr)/sizeof(g_po_arr[0]);

//
//  Private functions
//
char* convertToDecimal(char*, char*);

//
//  API functions
//
int plugin_get_info(struct plugin_info* ppi) {
    if (!ppi) {
        fprintf(stderr, "ERROR: invalid argument\n");
        return -1;
    }
    ppi->plugin_purpose = g_plugin_purpose;
    ppi->plugin_author = g_plugin_author;
    ppi->sup_opts_len = g_po_arr_len;
    ppi->sup_opts = g_po_arr;

    return 0;
}

int plugin_process_file(const char *fname,
        struct option in_opts[],
        size_t in_opts_len) {
    
    char *debug = getenv("LAB1DEBUG");
    if (!fname || !in_opts || !in_opts_len) {
        errno = EINVAL;
        return -1;
    }

    if (debug) {
        for (size_t i = 0; i < in_opts_len; i++) {
            fprintf(stderr, "debug: %s: Got option '%s' with arg '%s'\n",
                g_lib_name, in_opts[i].name, (char*)in_opts[i].flag);
        }
    }

    FILE *fp;
    for (size_t i = 0; i < in_opts_len; i++) {
        if (!strcmp(in_opts[i].name, OPT_BIT_SEQ_STR)) {
            char* target_not_convert = (char *)in_opts[i].flag;
            char* target = convertToDecimal(debug, target_not_convert);
            
            // Открываем файл для чтения
            fp = fopen(fname, "rb");
            if (fp == NULL) {
                fprintf(stderr, "ERROR: Error opening file %s: %s\n", fname, strerror(errno));
                return -1;
            }
            // Получаем размер файла
            fseek(fp, 0, SEEK_END);
            long fsize = ftell(fp);
            rewind(fp);
            
            // Выделяем память для хранения содержимого файла
            char *content = malloc(fsize + 1);
            if (content == NULL) {
                fprintf(stderr, "ERROR: Error allocating memory: %s\n", strerror(errno));
                fclose(fp);
                return -1;
            }

            // Читаем содержимое файла
            if (fread(content, fsize, 1, fp) != 1) {
                fprintf(stderr, "ERROR: Error reading file %s: %s\n", fname, strerror(errno));
                free(content);
                fclose(fp);
                return -1;
            }

            // Добавляем завершающий нулевой символ
            content[fsize] = '\0';

            // Ищем последовательность байтов в содержимом файла
            char *result = strstr(content, target);
            free(target);
            if (result != NULL) {
                // Вычисляем позицию, где найдена последовательность
                long pos = result - content;
                if (debug) printf("debug: The target sequence is found at position %ld\n", pos);
                // Освобождаем выделенную память и закрываем файл
                free(content);
                fclose(fp);
                return 0;
            }
            else {
                // Освобождаем выделенную память и закрываем файл
                free(content);
                fclose(fp);
                return 1;
            }
        }
        else {
            errno = EINVAL;
            return -1;
        }
    }
    return 0;
}

char* convertToDecimal(char* debug, char* number) {
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
        if (debug) fprintf(stderr, "debug: The target in decimal notation: %s\n", (char*)number);
        return (char*)number;
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
            fprintf(stderr, "ERROR: Invalid number format. Non-numeric digit found.\n");
            exit(EXIT_FAILURE);
        }

        decimal += value * pow(base, len - i - 1);
    }
    int len1 = 0;
    int tmp = decimal;
    while(tmp) {
        len1++;
        tmp /= 10;
    }
    char* decimal_str = (char*)malloc((len1 + 1) * sizeof(char));
    sprintf(decimal_str, "%d", decimal);
    if (debug) fprintf(stderr, "debug: The target in decimal notation: %s\n", decimal_str);
    return decimal_str;
}
