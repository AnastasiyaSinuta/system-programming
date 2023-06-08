#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <getopt.h>
#include <dlfcn.h>
#include <fts.h>

#include "plugin_api.h"

/*int plugWork(char*, char*);
struct env_options
{
    // NEED_MAKE
    char* pathToPlugin;
    char* option; // нужно обрезать от названия опции --
    char* target; // нужно поимать есть ли у опции аргумент
};*/

//typedef int (*prFunc_t)(const char* name, struct option in_opts[], size_t in_opts_len);

int main(int argc, char *argv[]) {
    char* debug = getenv("LAB1DEBUG");
    if (debug) fprintf(stderr, "debug: Debug mode enabled.\n");
    if (argc < 2) {
        fprintf(stderr, "Using: %s <dir> <target>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //char* dirWithPlugins = get_current_dir_name();
    char* dirWithPlugins = "/home/anastasiya/Desktop/system-programming/lab12aasN32511";
    //char* pathToLib = "/home/anastasiya/Desktop/system-programming/lab12aasN32511/libaasN32511.so";
    //char* target_not_convert = strdup(argv[argc - 2]);
    //char* directory = strdup(argv[argc-1]);
    //struct env_options *all_opt_from_env;
    int O_mode = 0;
    int N_mode = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-v")) {
            if (debug) fprintf(stderr, "debug: \"Version\" option.\n");
            fprintf(stderr, "Progamm:\tLab №1.2 - \"Using Dynamic Libraries\"\n");
            fprintf(stderr, "Version:\t1.0\n");
            fprintf(stderr, "Autor:\tSinuta Anastasiya Anatolevna\n");
            fprintf(stderr, "Group:\tN32511\n");
            fprintf(stderr, "Variant:\t5\n");
            if (debug) fprintf(stderr, "debug: End debugging.\n");
            exit(EXIT_SUCCESS);
        }
        if (!strcmp(argv[i], "-h")) {
            if (debug) fprintf(stderr, "debug: \"Help\" option.\n");
            fprintf(stderr, "Usage: %s [options] [directory]...\n\n", argv[0]);
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "\t-P <dir> Directory with plugins..\n"),
            fprintf(stderr, "\t-A Combining Plugin Options with an AND Operation (valid by default).\n");
            fprintf(stderr, "\t-O Combining plugin options using the OR operation.\n");
            fprintf(stderr, "\t-N Invert search condition (after merging options plugins with -A or -O).\n");
            fprintf(stderr, "\t-v Displaying the version of the program and information about the program (full name performer, group number, laboratory version number).\n");
            fprintf(stderr, "\t-h Display help for options.\n");
            printf("\nTarget:\n");
            printf("\tThe value of the sequence is given by a string containing a number entry,\n\teither in binary (0b...), or in decimal or hexadecimal (0x...) systems.\n\tThe length of the bit sequence can be arbitrary.\n");
            printf("\nDirectory:\n");
            printf("\tSpecify the path to the directory, starting from the directory \'/home\', from which you want to start the search.\n");
            printf("\nThis program built for x86_64-pc-linux-gnu\n");
            printf("Report bugs to <336972@niuitmo.ru>\n");
            if (debug) fprintf(stderr, "debug: End debugging.\n");
            exit(EXIT_SUCCESS);
        }
        if (!strcmp(argv[i], "-O")) O_mode = 1;
        if (!strcmp(argv[i], "-N")) N_mode = 1;
        if (!strcmp(argv[i], "-P")) {
            if (debug) fprintf(stderr, "debug: \"-P\" option.\n");
            if (i == argc-1) {
                fprintf(stderr, "ERROR: Cannot found path to plugin\n");
                exit(EXIT_FAILURE);
            }
            dirWithPlugins = strdup(argv[i+1]);
            if (debug) fprintf(stderr, "debug: Searching plugins in directory: %s\n", dirWithPlugins);
        }
    }
    char *paths[2] = {(char*)dirWithPlugins, NULL};
    FTS *fts_h = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (!fts_h) {
        fprintf(stderr, "fts_open() failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    while (1) {
        errno = 0;
        FTSENT *ent = fts_read(fts_h);
        if (ent == NULL) {
            if (errno != 0)
                continue;
            else break;
        }
        char* rasshirenie = strrchr(ent->fts_name, '.');
        if (debug) fprintf(stderr, "debug: Found file \"%s\", wist rasshir %s\n", ent->fts_name, rasshirenie);
        if (rasshirenie == ".so") {
            fprintf(stderr, "plugin: %s\n", ent->fts_name);
        }
        else {
            if (debug) fprintf(stderr, "ne plugin\n");
        }
    }
    fts_close(fts_h);

    /*NEED_MAKE
    // Найти опции, для которых путь к плагину не задан явно (через -P)
    // countOpt++;
    // Внести их в all_opt_from_env.

    // NEED_MAKE
    // Дальнейшая логика:
    // walk_dir по указанной директории
    // case FTS_F: допустим filename
    // if (N_mode=0) {
    //     if (O_mode=0) { //опиция И
    //         int file_podhodit = 1;
    //         for (option : all_opt_from_env) {
    //             if (plugWork(option, filename) == 1) { //плагин сказал что файл не подходит
    //                 file_podhodit = 0;  //значит нам этот файл не нужен вовсе
    //                 break;
    //             }
    //         }
    //         if (file_podhodit) printf(filename);
    //     }
    //     else { // опция ИЛИ
    //         for (option : all_opt_from_env) {
    //             // если хотя бы по одному плагину подходит то выводим
    //             if (plugWork(option, filename) == 0) { 
    //                 printf(filename);
    //                 break;
    //             }
    //         }
    //     }
    // }
    // else {
    //     if (O_mode=0) {
    //         int file_podhodit = 1;
    //         for (option : all_opt_from_env) {
    //             if (plugWork(option, filename) == 0) {
    //                 file_podhodit = 0;
    //                 break;
    //             }
    //         }
    //         if (file_podhodit) printf(filename);
    //     }
    //     else {        
    //         for (option : all_opt_from_env) {
    //             if (plugWork(option, filename) == 1) { 
    //                 printf(filename);
    //                 break;
    //             }
    //         }
    //     }
    // }
*/
    /*fprintf(stderr, "lib: %s\n", pathToLib);
    fprintf(stderr, "target: %s\n", target_not_convert);
    char* target = convertToDecimal(target_not_convert);

    // Загрузка разделяемой библиотеки
    void *handle = dlopen(pathToLib, RTLD_LAZY);
    if (handle == NULL) {
        fprintf(stderr, "Не удалось загрузить библиотеку: %s\n", dlerror());
        return EXIT_FAILURE;
    }
    // Загрузка функции из библиотеки
    prFunc_t plugin_process_file = (prFunc_t)dlsym(handle, "plugin_process_file");
    if (plugin_process_file == NULL) {
        fprintf(stderr, "Не удалось найти символ: %s\n", dlerror());
        return EXIT_FAILURE;
    }
    const char* dir = strdup(argv[argc - 1]);
    char *paths[2] = {(char*)dir, NULL};
    FTS *fts_h = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (!fts_h) {
        fprintf(stderr, "fts_open() failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    int count = 0;
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
                if (debug) fprintf(stderr, "debug: Entering the \"%s\" directory\n", ent->fts_name);
                break;
            case FTS_DC:        // Каталог, вызвавший зацикливание по дереву
                if (debug) fprintf(stderr, "debug: Ignoring cycle directory \"%s\"\n", ent->fts_name);
                break;
            case FTS_DEFAULT:   // Любая структура Fa FTSENT, представляющая тип файла, неявно описанного одним из значений Fa fts_info
            case FTS_DNR:       // Каталог, который не может быть прочитан. Это значение возвращается при ошибке, и поле Fa fts_errno будет заполнено тем, что вызвало ошибку
                fprintf(stderr, "%s: %s\n", ent->fts_name, strerror(ent->fts_errno));
                break;
            case FTS_DOT:       // Файл, названный `.' или `..' который не был определен как файловое имя в fts_open ()
            case FTS_DP:        // Каталог, посещаемый в обратном порядке. Содержимое структуры Fa FTSENT будет неизменно, как если-бы он посещался в прямом порядке, то есть с полем Fa fts_info, установленным в FTS_D
                if (debug) fprintf(stderr, "debug: Exiting directory\"%s\"\n", ent->fts_name);
                break;
            case FTS_ERR:       // Это значение возвращается при ошибке, и поле Fa fts_errno будет заполнено тем, что вызвало ошибку
                fprintf(stderr, "%s: %s\n", ent->fts_name, strerror(ent->fts_errno));
                break;
            case FTS_F:         // Простой файл
                if (debug) fprintf(stderr, "debug: Found file \"%s\"\n", ent->fts_name);
                //struct option *long_opts;
                struct option opt;// = long_opts[loptInd[i]];
                char* arg = target;
                if (arg) {
                    opt.has_arg = 1;
                    opt.flag = (void*)arg;
                }
                else {
                    opt.has_arg = 0;
                }
                if (!plugin_process_file(ent->fts_path, &opt, 1)) {
                    printf("File %s contains the target sequence of bytes\n", ent->fts_name);
                }
                else {
                    if (debug) printf("debug: File %s does not contain the target sequence of bytes\n", ent->fts_name);
                }
                break;
            case FTS_NS:        // Файл, для которого нет доступной информации stat(2). Содержимое поля Fa fts_statp не определено. Это значение возвращается при ошибке, и поле Fa fts_errno будет заполнено тем, что вызвало ошибку
                fprintf(stderr, "%s: %s\n", ent->fts_name, strerror(ent->fts_errno));
                break;
            case FTS_NSOK:      // Файл, для которого информация о stat(2) не была запрошена. Содержимое поля Fa fts_statp не определено
                break;
            case FTS_SL:        // Символьная ссылка
                break;
            case FTS_SLNONE:    // Символьная ссылка, указывающая на несуществующий объект. Содержимое поля Fa fts_statp определяет ссылку на информацию о свойствах файла для самой символьной ссылки
                if (debug) fprintf(stderr, "debug: Ignoring link \"%s\"\n", ent->fts_name);
                break;
        }
    }   
    fts_close(fts_h);
    if (debug) fprintf(stderr, "debug: End debugging.\n");
    dlclose(handle); // Закрытие библиотеки*/
    
    return EXIT_SUCCESS;
}
