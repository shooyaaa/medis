#include <string.h>
#include <stdlib.h>
#include "resp.h"

inline int findCr(char * buffer, int start, int size) {
    int ret = -1;
    for (int i = start; i <= size; i ++) {
        if (buffer[i] == '\n') {
            ret = i;
            break;
        }
    }
    return ret;
}

int parse(char *buffer, int start, int size, resp *rsp) {
    if (start >= size) return 0;
    char type = buffer[start];
    int i, len;
    char *s;
    int crlf = findCr(buffer, start, size);
    char *value = (char *)malloc(crlf);
    strncpy(value, buffer + start + 1, crlf - start);
    switch (type) {
        case '+' :
            rsp->s = value;
            rsp->type = RESP_STRING;
            break;
        case '-' :
            rsp->s = value;
            rsp->type = RESP_STRING;
            break;
        case ':' :
            rsp->i = atoi(value);
            rsp->type = RESP_INT;
            break;
        case '$' :
            len = atoi(value);
            value = (char *)malloc(len);
            strncpy(value, buffer + start + len + 3, len);
            rsp->s = value;
            rsp->type = RESP_STRING;
            rsp->size = len;
            break;
        case '*':
            len = atoi(value);
            parse(buffer, start + crlf, size, *rsp->arr);
            rsp->type = RESP_ARRAY;
            rsp->size = len;
            break;
    }
    return 1;
}

int serialize(resp *rp, char *ret) {

    return 1;
}
