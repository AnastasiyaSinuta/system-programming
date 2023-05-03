#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
 
void found_file(int debug, FTSENT *ent, const char* argv_target) {
    if (debug) fprintf(stderr, "DEBUG: Found file \"%s\"\n", ent->fts_name);
    // Открываем файл для чтения
    FILE *fp = fopen(ent->fts_path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file %s: %s\n", ent->fts_path, strerror(errno));
        return;
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
        return;
    }
    
    // Читаем содержимое файла
    if (fread(content, fsize, 1, fp) != 1) {
        fprintf(stderr, "Error reading file %s: %s\n", ent->fts_path, strerror(errno));
        free(content);
        fclose(fp);
        return;
    }
    
    // Добавляем завершающий нулевой символ
    content[fsize] = '\0';
    
    // Ищем последовательность байтов в содержимом файла
    char *result = strstr(content, argv_target);
    if (result != NULL) {
        // Вычисляем позицию, где найдена последовательность
        long pos = result - content;
        printf("File %s contains the target sequence of bytes at position %ld\n", ent->fts_path, pos);
    }
    else {
        if (debug) printf("File %s does not contain the target sequence of bytes\n", ent->fts_path);
    }
    
    // Освобождаем выделенную память и закрываем файл
    free(content);
    fclose(fp);
}

void walk_dir(int debug, const char* argv_dir, const char* argv_target) {
    char *paths[2] = {(char*)argv_dir, NULL};
    FTS *fts_h = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (!fts_h) {
        fprintf(stderr, "fts_open() failed: %s\n", strerror(errno));
        return;
    }
    while (1) {
        errno = 0;
        FTSENT *ent = fts_read(fts_h);
        if (ent == NULL) {
            if (errno != 0)
                continue;   // Проблема, переходим к следующему элементу
            else
                break;      // В каталоге больше ничего нет
        }
        switch (ent->fts_info) {
            case FTS_D:         // Каталог, посещаемый в прямом порядке
                if (debug) fprintf(stderr, "DEBUG: Entering directory\"%s\"\n", ent->fts_name);
                break;
            case FTS_DC:        // Каталог, вызвавший зацикливание по дереву
                if (debug) fprintf(stderr, "DEBUG: Ignoring cycle directory \"%s\"\n", ent->fts_name);
                break;
            case FTS_DEFAULT:   // Любая структура Fa FTSENT, представляющая тип файла, неявно описанного одним из значений Fa fts_info
            case FTS_DNR:       // Каталог, который не может быть прочитан. Это значение возвращается при ошибке, и поле Fa fts_errno будет заполнено тем, что вызвало ошибку
                fprintf(stderr, "%s: %s\n", ent->fts_name, strerror(ent->fts_errno));
                break;
            case FTS_DOT:       // Файл, названный `.' или `..' который не был определен как файловое имя в fts_open ()
            case FTS_DP:        // Каталог, посещаемый в обратном порядке. Содержимое структуры Fa FTSENT будет неизменно, как если-бы он посещался в прямом порядке, то есть с полем Fa fts_info, установленным в FTS_D
                if (debug) fprintf(stderr, "DEBUG: Exiting directory\"%s\"\n", ent->fts_name);
                break;
            case FTS_ERR:       // Это значение возвращается при ошибке, и поле Fa fts_errno будет заполнено тем, что вызвало ошибку
                fprintf(stderr, "%s: %s\n", ent->fts_name, strerror(ent->fts_errno));
                break;
            case FTS_F:         // Простой файл
                found_file(debug, ent, argv_target);
                break;
            case FTS_NS:        // Файл, для которого нет доступной информации stat(2). Содержимое поля Fa fts_statp не определено. Это значение возвращается при ошибке, и поле Fa fts_errno будет заполнено тем, что вызвало ошибку
                fprintf(stderr, "%s: %s\n", ent->fts_name, strerror(ent->fts_errno));
                break;
            case FTS_NSOK:      // Файл, для которого информация о stat(2) не была запрошена. Содержимое поля Fa fts_statp не определено
                break;
            case FTS_SL:        // Символьная ссылка
                break;
            case FTS_SLNONE:    // Символьная ссылка, указывающая на несуществующий объект. Содержимое поля Fa fts_statp определяет ссылку на информацию о свойствах файла для самой символьной ссылки
                if (debug) fprintf(stderr, "DEBUG: Ignoring link \"%s\"\n", ent->fts_name);
                break;
        }
    }   
    fts_close(fts_h);
    if (debug) fprintf(stderr, "DEBUG: Done. Quitting.\n");
}