#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fts.h>
#include <errno.h>
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
int debug_mode();
int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) {
    int debug = debug_mode();
    if (! fname || !in_opts || in_opts_len <= 0){
        errno = EINVAL;
        return -1;
    }
    FILE *fp;
    for (size_t i = 0; i < in_opts_len; i++) {
        char* target = (char *)in_opts[i].flag;
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
            if (debug) printf("debug: Found at position %ld\n", pos);
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
    return 0;
}
