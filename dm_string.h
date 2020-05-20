#ifndef __dm_string__
#define __dm_string__
typedef struct _dm_string {
    int size;
    int cap;
    char *str;
} dmString;

#define DMSIZE(dm) (dm)->size

void dmInit(dmString *dm);
void dmAppend(dmString *dm, char *s, int size);
void dmMerge(dmString *src, dmString *dst);
#endif
