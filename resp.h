#ifndef __resp__
#define __resp__

#define RESP_INT    0
#define RESP_STRING 1
#define RESP_ERROR  2
#define RESP_ARRAY  3
#define RESP_BULK   4
#define RESP_SIMPLE 5

#include "dm_string.h"

typedef struct _resp {
    char type;
    int size;
    union {
       long long i;
       char *s;
       struct _resp **arr;
    };
    struct _resp *next;
} resp;

#define IS_INT(v) (v)->type == RESP_INT

#define IS_STRING(v) (v)->type == RESP_STRING

#define IS_ERROR(v) (v)->type == RESP_ERROR

#define IS_ARRAY(v) (v)->type == RESP_ARRAY

#define IS_BULK(v) (v)->type == RESP_BULK

int parse(char *buffer, int start, int size, resp *rsp);
int serialize(resp *rp, dmString *dm);
int findCr(char *buffer, int start, int size);

#endif
