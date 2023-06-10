#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <getopt.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fts.h>

#include "plugin_api.h"

struct Plugin {
    void* file;
    int numberOptions;
    struct plugin_option *opts;
};
struct Plugin *plugins;
int pluginsSize = 0;
struct option *long_opts;

int main(int argc, char *argv[]) {
    char* dirWithPlugins = get_current_dir_name();
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-v")) {
            fprintf(stderr, "Lab №1.2 - \"Using Dynamic Libraries\"\n");
            fprintf(stderr, "Version: 1.0\n");
            fprintf(stderr, "Autor: Sinuta Anastasiya Anatolevna\n");
            fprintf(stderr, "Group: N32511\n");
            fprintf(stderr, "Variant: 5\n");
            exit(EXIT_SUCCESS);
        }
        if (!strcmp(argv[i], "-h")) {
            fprintf(stderr, "Usage: %s [options] [directory]...\n", argv[0]);
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "-P <dir> Directory with plugins..\n"),
            fprintf(stderr, "-A Combining Plugin Options with an AND Operation (valid by default).\n");
            fprintf(stderr, "-O Combining plugin options using the OR operation.\n");
            fprintf(stderr, "-N Invert search condition (after merging options plugins with -A or -O).\n");
            fprintf(stderr, "-v Displaying the version of the program and information about the program (full name performer, group number, laboratory version number).\n");
            fprintf(stderr, "-h Display help for options.\n");
            exit(EXIT_SUCCESS);
        }
        if (!strcmp(argv[i], "-P")) {
            if (i == argc-1) {
                fprintf(stderr, "ERROR: Option -P needs an argument <dir>\n");
                exit(EXIT_FAILURE);
            }
            dirWithPlugins = strdup(argv[i+1]);
        }
    }

    // Включение модуля отладки
    char* debug = getenv("LAB1DEBUG");
    if (debug) fprintf(stderr, "debug: Debug mode enabled.\n");
    if (argc < 2) {
        fprintf(stderr, "Using: %s <dir> <target>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Поиск плагина
    if (debug) fprintf(stderr, "debug: Searching plugins in directory: %s\n", dirWithPlugins);
    FTS *ftsp;
    FTSENT *entry;
    
    char *paths[] = { dirWithPlugins, NULL };
    
    ftsp = fts_open(paths, FTS_NOCHDIR, NULL);
    if (ftsp == NULL) {
        perror("fts_open");
        exit(EXIT_FAILURE);
    }

    // Создаем массив указателей на строки для хранения путей файлов
    char **filePaths = NULL;
    size_t filePathsSize = 0;
    
    while ((entry = fts_read(ftsp)) != NULL) {
        if (entry->fts_info & FTS_F && strcmp(entry->fts_name + entry->fts_namelen - 3, ".so") == 0) {
            // Выделяем память для пути файла
            char *path = strdup(entry->fts_path);
            if (path == NULL) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }

            // Увеличиваем размер массива и сохраняем путь файла в него
            filePathsSize++;
            filePaths = realloc(filePaths, filePathsSize * sizeof(char *));
            if (filePaths == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
            filePaths[filePathsSize - 1] = path;
            if (debug) fprintf(stderr, "debug: Found shared library: %s\n", entry->fts_name);
        }
    }
    
    if (errno) {
        perror("fts_read");
        exit(EXIT_FAILURE);
    }
    
    fts_close(ftsp);

    plugins = (struct Plugin*)malloc(filePathsSize * sizeof(struct Plugin));
    if (plugins == 0) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    pluginsSize = filePathsSize;
    for (int i = 0; i < pluginsSize; i++) {
        void *dl = dlopen(filePaths[i], RTLD_LAZY);
        free(filePaths[i]);
        if (!dl) {
            fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }

        // Check for plugin_get_info() func
        void *func = dlsym(dl, "plugin_get_info");
        if (!func) {
            fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
            exit(EXIT_FAILURE);
        }

        typedef int (*pgi_func_t)(struct plugin_info*);
        pgi_func_t pgi_func = (pgi_func_t)func;            

        struct plugin_info pi = {0};
        int ret = pgi_func(&pi);
        if (ret < 0) {        
            fprintf(stderr, "ERROR: plugin_get_info() failed\n");
            exit(EXIT_FAILURE);
        }

        if (debug) // Plugin info 
        {
            fprintf(stdout, "Plugin purpose:\t\t%s\n", pi.plugin_purpose);
            fprintf(stdout, "Plugin author:\t\t%s\n", pi.plugin_author);
            fprintf(stdout, "Supported options: ");
            if (pi.sup_opts_len > 0) {
                fprintf(stdout, "\n");
                for (size_t i = 0; i < pi.sup_opts_len; i++) {
                    fprintf(stdout, "\t--%s\t\t%s\n", pi.sup_opts[i].opt.name, pi.sup_opts[i].opt_descr);
                }
            }
            else fprintf(stdout, "none (!?)\n");
            fprintf(stdout, "\n");
        }
        
        // If library supports no options then we have to stop
        if (pi.sup_opts_len == 0) {
            fprintf(stderr, "ERROR: library supports no options! How so?\n");
            exit(EXIT_FAILURE);
        }

        plugins[i].file = dl;
        plugins[i].numberOptions = pi.sup_opts_len;
        plugins[i].opts = pi.sup_opts;
    }
    free(filePaths);
    
    size_t opt_count = 0;
    for (int i = 0; i < pluginsSize; i++) {
        opt_count += plugins[i].numberOptions;
    }

    long_opts = (struct option*)malloc(opt_count * sizeof(struct option));
    if (!long_opts) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    opt_count = 0;
    for (int i = 0; i < pluginsSize; i++) {
        for (int j = 0; j < plugins[i].numberOptions; j++) {
            long_opts[opt_count] = plugins[i].opts[j].opt;
            opt_count++;
        }
    }
    
    int O_mode = 0;
    int N_mode = 0;
    int ret;
    while (1) {
        int opt_ind = 0;
        ret = getopt_long(argc, argv, "P:AON", long_opts, &opt_ind);
        switch (ret) {
        case -1: break;
        case 'P': break; 
        case 'A': break;
        case 'O':
            O_mode = 1;
            break;
        case 'N':
            N_mode = 1;
        case ':': exit(EXIT_FAILURE);
        case '?': exit(EXIT_FAILURE);
        default:
            
        }
    }
    //char* directory = strdup(argv[argc-1]);

    /*NEED_MAKE
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
