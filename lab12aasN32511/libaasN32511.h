#ifndef _LIB_AAS_N32511_H
#define _LIB_AAS_N32511_H

#include <getopt.h>
#include <stdio.h>
#include <errno.h> 

#define LIB_AAS_N32511_VERSION  1

/*
    Структура, описывающая опцию, поддерживаемую плагином.
*/
struct plugin_option {
    /* Опция в формате, поддерживаемом getopt_long (man 3 getopt_long). */
    struct option opt;
    /* Описание опции, которое предоставляет плагин. */
    const char *opt_descr;
};

/*
    Структура, содержащая информацию о плагине.
*/
struct plugin_info {
    /* Назначение плагина */
    const char *plugin_purpose;
    /* Автор плагина, например "Иванов Иван Иванович, N32xx" */
    const char *plugin_author;
    /* Длина списка опций */
    size_t sup_opts_len;
    /* Список опций, поддерживаемых плагином */
    struct plugin_option *sup_opts;
};

int plugin_get_info(struct plugin_info* ppi);
/*
    plugin_get_info()
    
    Функция, позволяющая получить информацию о плагине.
    
    Аргументы:
        ppi - адрес структуры, которую заполняет информацией плагин.
        
    Возвращаемое значение:
          0 - в случае успеха,
        < 0 - в случае неудачи (в этом случае продолжать работу с этим плагином нельзя).
*/

int walk_dir(const char *fname);

int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) {
    if (walk_dir(fname) == 1) {
        return 0;
    } else {
        return 1;
    }

    // В случае ошибки:
    // errno = соответствующее_значение;
    // return -1;
}
     
#endif
