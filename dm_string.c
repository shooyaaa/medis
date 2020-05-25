#include <stdlib.h>
#include <string.h>
#include "defines.h"

void dmInit(dmString *dm) {
    dm->cap = 16;
    dm->size = 0;
    dm->str = calloc(1, dm->cap);
}

void dmAppend(dmString *dm, char *s, int size) {
    if (dm->size + size > dm->cap) {
        dm->cap <<= 1;
        dm->cap = dm->cap ? dm->cap : 4;
        char *bigger = calloc(1, dm->cap);
        if (dm->size) {
            strncpy(bigger, dm->str, dm->size);
            free(dm->str);
        }
        dm->str = bigger;
    }
    strncpy(dm->str + dm->size, s, size);
    dm->size += size;
}


void dmMerge(dmString *src, dmString *dst) {
    dmAppend(src, dst->str, dst->size);
}
