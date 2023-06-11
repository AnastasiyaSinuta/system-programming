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

int main(int argc, char *argv[]) {
    int A_mode = 1;
    int O_mode = 0;
    int N_mode = 0;
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
        if (!strcmp(argv[i], "-O")) {
            O_mode = 1;
            A_mode = 0;
        }
        if (!strcmp(argv[i], "-N")) N_mode = 1;
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

    struct Plugin {
        int index;
        char* file;
        int numberOptions;
        struct plugin_option *opts;
    };

    int pluginsSize = filePathsSize;
    struct Plugin *plugins = (struct Plugin*)malloc(filePathsSize * sizeof(struct Plugin));
    if (plugins == 0) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    void *dl;
    for (int i = 0; i < pluginsSize; i++) {
        dl = dlopen(strdup(filePaths[i]), RTLD_LAZY);
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
            fprintf(stdout, "\nPlugin purpose:\t\t%s\n", pi.plugin_purpose);
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
        plugins[i].index = i;
        plugins[i].file = strdup(filePaths[i]);
        plugins[i].numberOptions = pi.sup_opts_len;
        plugins[i].opts = pi.sup_opts;
    }
    free(filePaths);
    
    size_t opt_count = 0;
    for (int i = 0; i < pluginsSize; i++) {
        opt_count += plugins[i].numberOptions;
    }
    struct option *long_opts;
    int *opt_plug_index;
    long_opts = calloc(opt_count, sizeof(struct option));
    opt_plug_index = calloc(opt_count, sizeof(int));
    opt_count = 0;

    // Считываем опции, заданные командной строкой
    for (int op = optind; op < argc; op++) {
        char* obrStr = strdup(argv[op]);
        int len = strlen(obrStr);
        if (len <= 2) continue;;
        memmove(obrStr, obrStr + 2, len - 1);
        obrStr[len - 2] = '\0';
        for (int i = 0; i < pluginsSize; i++) {
            for (int j = 0; j < plugins[i].numberOptions; j++) {
                char* name = (char*)plugins[i].opts[j].opt.name;
                if (!strcmp(obrStr, name)){
                    if (debug) fprintf(stderr, "debug: In env found option: %s\n", name);
                    long_opts[opt_count] = plugins[i].opts[j].opt;
                    if (long_opts[opt_count].has_arg)
                        long_opts[opt_count].flag = (void*)argv[op+1];
                    opt_plug_index[opt_count] = plugins[i].index;
                    opt_count++;
                }
            }
        }
    }

    char* directory_for_search = strdup(argv[argc - 1]);
    if (debug) fprintf(stderr, "debug: Searching files in directory: %s\n", directory_for_search);

    void *func;
    typedef int (*ppf_func_t)(const char*, struct option*, size_t);

    int result;

    char *pathDir[2] = {(char*)directory_for_search, NULL};
    FTS *fts_h = fts_open(pathDir, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (!fts_h) {
        fprintf(stderr, "fts_open() failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    while (1) {
        errno = 0;
        FTSENT *ent = fts_read(fts_h);
        if (ent == NULL) {
            if (errno != 0) continue;   // Проблема, переходим к следующему элементу
            else break;      // В каталоге больше ничего нет
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
                int suitableAND = 1, suitableOR = 1;
                for (int i = 0; i < pluginsSize; i++) {
                    dl = dlopen(strdup(plugins[i].file), RTLD_LAZY);
                    if (!dl) {
                        fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
                        exit(EXIT_FAILURE);
                    }
                    func = dlsym(dl, "plugin_process_file");
                    if (!func) {
                        fprintf(stderr, "ERROR: no plugin_process_file() function found\n");
                        exit(EXIT_FAILURE);
                    }
                    size_t in_opts_len = plugins[i].numberOptions;
                    struct option *in_opts;
                    in_opts = calloc(in_opts_len, sizeof(struct option));
                    in_opts_len = 0;
                    for (int j = 0; j < opt_count; j++) {
                        if (opt_plug_index[j] == plugins[i].index) {
                            in_opts[in_opts_len] = long_opts[j];
                            in_opts_len++;
                        }
                    }
                    ppf_func_t ppf_func = (ppf_func_t)func;
                    result = ppf_func(ent->fts_path, in_opts, in_opts_len);
                    if (debug) fprintf(stderr, "debug: plugin_process_file() returned %d\n", result);
                    if (result == -1) {
                        fprintf(stderr, "ERROR: An error occurred while running the plugin\n");
                        exit(EXIT_FAILURE);
                    }
                    else if (!O_mode && ((!N_mode && result) || (N_mode && !result))) {
                        suitableAND = 0;
                        break;
                    }
                    else if (O_mode && ((!N_mode && !result) || (N_mode && result))) {
                        suitableOR = 1;
                        break;
                    }
                }
                if (suitableAND && suitableOR) {
                    fprintf(stderr, "Found a suitable file: %s\n", ent->fts_path);
                }
                else if (debug) fprintf(stderr, "debug: File does not match: %s\n", ent->fts_path);
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
    return EXIT_SUCCESS;
}
