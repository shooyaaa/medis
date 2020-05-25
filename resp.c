#include <string.h>
#include <stdlib.h>
#include "resp.h"
#include "defines.h"

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
    int crlf = findCr(buffer, start, size);
    int i, len = crlf - start;
    char *s;
    char *value = (char *)malloc(len);
    resp **temp;
    strncpy(value, buffer + start + 1, crlf - start);
    switch (type) {
        case '+' :
            rsp->s = value;
            rsp->type = RESP_STRING;
            rsp->size = len;
            break;
        case '-' :
            rsp->s = value;
            rsp->type = RESP_ERROR;
            rsp->size = len;
            break;
        case ':' :
            rsp->i = atoi(value);
            rsp->type = RESP_INT;
            break;
        case '$' :
            len = atoi(value);
            value = (char *)malloc(len);
            strncpy(value, buffer + crlf + 1, len);
            rsp->s = value;
            rsp->type = RESP_BULK;
            rsp->size = len;
            crlf += 1 + len;
            break;
        case '*':
            len = atoi(value);
            rsp->type = RESP_ARRAY;
            rsp->size = len;
            rsp->arr = (resp **)calloc(len, sizeof(resp*));
            crlf ++;
            for (int i = 0; i < len; i ++) {
                rsp->arr[i] = calloc(1, sizeof(resp));
                crlf = parse(buffer, crlf, size, rsp->arr[i]);
            }
            crlf -= 2;
            break;
        default:
            i = start;
            rsp->type = RESP_ARRAY;
            rsp->size = 0;
            while (i++ <= size) {
                if (buffer[i] == ' ' || i == size) {
                    rsp->size ++;
                }
            }
            rsp->arr = (resp **)calloc(rsp->size, sizeof(resp*));
            i = start;
            temp = rsp->arr;
            while (i++ < size) {
                if (buffer[i] == ' ' || buffer[i] == '\r') {
                    *temp = calloc(1, sizeof(resp));
                    (*temp)->type = RESP_STRING;
                    value = (char *)malloc(i - start);
                    strncpy(value, buffer + start, i - start);
                    (*temp)->s = value;
                    (*temp)->size = i - start;
                    start = i;
                    temp ++;
                }
            }
            break;
    }
    return crlf + 2;
}

int serialize(resp *rp, dmString *dm) {
    char *temp = calloc(1, 50);
    switch (rp->type) {
        case RESP_INT:
            sprintf(temp, "%lld", rp->i);
            dmAppend(dm, ":", 1);
            dmAppend(dm, temp, strlen(temp));
            dmAppend(dm, "\r\n", 2);
            break;
        case RESP_STRING:
            dmAppend(dm, "+", 1);
            dmAppend(dm, rp->s, rp->size);
            dmAppend(dm, "\r\n", 2);
            break;
        case RESP_ERROR:
            dmAppend(dm, "-", 1);
            dmAppend(dm, rp->s, rp->size);
            dmAppend(dm, "\r\n", 2);
            break;
        case RESP_BULK:
            dmAppend(dm, "$", 1);
            sprintf(temp, "%d", rp->size);
            dmAppend(dm, temp, strlen(temp));
            dmAppend(dm, "\r\n", 2);
            dmAppend(dm, rp->s, rp->size);
            dmAppend(dm, "\r\n", 2);
            break;
        case RESP_ARRAY:
            dmAppend(dm, "*", 1);
            sprintf(temp, "%d", rp->size);
            dmAppend(dm, temp, strlen(temp));
            dmAppend(dm, "\r\n", 2);
            for (int i = 0; i < rp->size; i++) {
                serialize(rp->arr[i], dm);
            }
            break;
        default:
            return -1;
    }
    if (rp->next) {
        serialize(rp->next, dm);
    }
    return 0;
}
