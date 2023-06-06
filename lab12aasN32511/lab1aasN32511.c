#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <getopt.h>
#include <dlfcn.h>
#include <fts.h>

#include "plugin_api.h"

int debug_mode();
void print_version(int);
void print_help(int, const char*);
char* convertToDecimal(char*);

typedef int (*prFunc_t)(const char* name, struct option in_opts[], size_t in_opts_len);

int main(int argc, char *argv[]) {
    int debug = debug_mode();
    if (debug==1) {
        fprintf(stderr, "debug: Debug mode enabled.\n");
    }
    else {
        fprintf(stderr, "debug: Debug mode not enabled.\n");
    }
    if (argc < 2) {
        fprintf(stderr, "Using: %s <dir> <target>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* pathToLib = "/home/anastasiya/Desktop/system-programming/lab12aasN32511/libaasN32511.so";
    char* target_not_convert = argv[argc - 2];
    int O_mode = 0;
    int N_mode = 0;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) print_version(debug);
        if (strcmp(argv[i], "-h") == 0) print_help(debug, argv[0]);
        if (strcmp(argv[i], "-O") == 0) O_mode = 1;
        if (strcmp(argv[i], "-N") == 0) N_mode = 1;
        if (strcmp(argv[i], "-P") == 0) {
            if (debug) fprintf(stderr, "debug: \"-P\" option.\n");
            if (i == argc-1) {
                fprintf(stderr, "error: Cannot found path to plugin\n");
                exit(EXIT_FAILURE);
            }
            else {
                // Name of the lib.
                char *lib_name = strdup(argv[i+1]);
                
                struct plugin_info pi = {0};
                
                void *dl = dlopen(argv[i+1], RTLD_LAZY);
                if (!dl) {
                    fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
                    return EXIT_FAILURE;
                }
                
                // Check for plugin_get_info() func
                void *func = dlsym(dl, "plugin_get_info");
                if (!func) {
                    fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
                    return EXIT_FAILURE;
                }
                
                typedef int (*pgi_func_t)(struct plugin_info*);
                pgi_func_t pgi_func = (pgi_func_t)func;            

                int ret = pgi_func(&pi);
                if (ret < 0) {        
                    fprintf(stderr, "ERROR: plugin_get_info() failed\n");
                    return EXIT_FAILURE;
                }

                // Plugin info       
                fprintf(stdout, "Plugin purpose:\t\t%s\n", pi.plugin_purpose);
                fprintf(stdout, "Plugin author:\t\t%s\n", pi.plugin_author);
                fprintf(stdout, "Supported options: ");
                if (pi.sup_opts_len > 0) {
                    fprintf(stdout, "\n");
                    for (size_t i = 0; i < pi.sup_opts_len; i++) {
                        fprintf(stdout, "\t--%s\t\t%s\n", pi.sup_opts[i].opt.name, pi.sup_opts[i].opt_descr);
                    }
                }
                else {
                    fprintf(stdout, "none (!?)\n");
                }
                fprintf(stdout, "\n");

                // If library supports no options then we have to stop
                if (pi.sup_opts_len == 0) {
                    fprintf(stderr, "ERROR: library supports no options! How so?\n");
                    return EXIT_FAILURE;
                }

                // Get pointer to plugin_process_file()
                func = dlsym(dl, "plugin_process_file");
                if (!func) {
                    fprintf(stderr, "ERROR: no plugin_process_file() function found\n");
                    return EXIT_FAILURE;
                }
                
                typedef int (*ppf_func_t)(const char*, struct option*, size_t);
                ppf_func_t ppf_func = (ppf_func_t)func;  
            }
        }
        /*if (strcmp(argv[i], "--bit-seq") == 0) {
            if (debug) fprintf(stderr, "debug: \"-bit-seq\" option.\n");
            if (i == argc-1) {
                fprintf(stderr, "Не найдено значение искомой последовательности битов\n");
                exit(EXIT_FAILURE);
            }
            else {
                target_not_convert = argv[i+1];
            }
        }*/
        
    }
    fprintf(stderr, "lib: %s\n", pathToLib);
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
    dlclose(handle); // Закрытие библиотеки
    return EXIT_SUCCESS;
}

void print_version(int debug) {
    if (debug) fprintf(stderr, "debug: \"Version\" option.\n");
    const char *Programm = "Lab №1.2 - \"Using Dynamic Libraries\"";
    char *Version = "1.0";
    const char *Autor = "Sinuta Anastasiya Anatolevna";
    const char *Group = "N32511";
    const int Variant = 5;
    printf("Progamm: %s\nVersion: %s\nAutor: %s\nGroup: %s\nVariant: %d\n", Programm, Version, Autor, Group, Variant);
    if (debug) fprintf(stderr, "debug: End debugging.\n");
    exit(EXIT_SUCCESS);
}

void print_help(int debug, const char* name) {
    if (debug) fprintf(stderr, "debug: \"Help\" option.\n");
    printf("Usage: %s [options] [target] [directory]...\n", name);
    printf("\nOptions:\n");
    const char* options[] = {
        "\t-P <dir> Directory with plugins..\n",
        "\t-A Combining Plugin Options with an AND Operation (valid by default).\n",
        "\t-O Combining plugin options using the OR operation.\n",
        "\t-N Invert search condition (after merging options plugins with -A or -O).\n",
        "\t-v Displaying the version of the program and information about the program (full name performer, group number, laboratory version number).\n",
        "\t-h Display help for options.\n"
    };
    int count = sizeof(options)/sizeof(options[0]);
    for (int i = 0; i < count; i++) {
        printf("%s", options[i]);
    }
    printf("\nTarget:\n");
    printf("\tThe value of the sequence is given by a string containing a number entry,\n\teither in binary (0b...), or in decimal or hexadecimal (0x...) systems.\n\tThe length of the bit sequence can be arbitrary.\n");
    printf("\nDirectory:\n");
    printf("\tSpecify the path to the directory, starting from the directory \'/home\', from which you want to start the search.\n");
    printf("\nThis program built for x86_64-pc-linux-gnu\n");
    printf("Report bugs to <336972@niuitmo.ru>\n");
    if (debug) fprintf(stderr, "debug: End debugging.\n");
    exit(EXIT_SUCCESS);
}
