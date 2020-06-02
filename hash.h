#ifndef __HASH__
#define __HASH__

#include "resp.h"

typedef struct __bucket {
    char *key;
    int len;
    resp *value;
    int expires;
    struct __bucket *next;
} bucket;

typedef int (*hashFunc)(char *key, int len, int size);
typedef struct __hash {
    int size;
    int count;
    bucket **head;
    hashFunc func;
} hash;

hash *newHash(int size);
int hashSet(hash *h, char *key, int len, resp *value, int expires);
_Bool hashDel(hash *h, char *key, int len);
resp *hashGet(hash *h, char *key, int len);
#endif
