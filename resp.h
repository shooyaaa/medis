#ifndef __resp__
#define __resp__

#define RESP_INT  0
#define RESP_STRING 1
#define RESP_ARRAY 2

typedef struct _resp {
    char type;
    int size;
    union {
       long long i;
       char *s;
       struct _resp **arr;
    };
} resp;

#define IS_INT(v) (v)->type == RESP_INT

#define IS_STRING(v) (v)->type == RESP_STRING

#define IS_ARRAY(v) (v)->type == RESP_ARRAY

int parse(char *buffer, int start, int size, resp *rsp);
int serialize(resp *rp, char *ret);
int findCr(char *buffer, int start, int size);

#endif
