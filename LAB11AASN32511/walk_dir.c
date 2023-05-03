#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

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
            case FTS_F:  // если это файл, то ищем заданную строку 
                if (strstr(ent->fts_name, argv_target) != NULL) { 
                    printf("%s\n", ent->fts_path); // выводим полный путь к файлу 
                } 
                break; 
            case FTS_D:  // если это каталог, то переходим в него 
                break; 
            case FTS_DC: 
            case FTS_DEFAULT: 
            case FTS_DNR: 
            case FTS_ERR: 
            case FTS_NS: 
                // произошла ошибка, выводим ее сообщение и завершаем программу 
                fprintf(stderr, "%s: %s\n", ent->fts_name, strerror(ent->fts_errno)); 
                exit(EXIT_FAILURE); 
                break; 
        } 
    }
    
    fts_close(fts_h);
}