#include <stdlib.h>
#include <string.h>
#include "hash.h"

int dummyHash(char *key, int len, int size) {
    int count = 0;
    int i = 0;
    while (i++ < len) {
        count += key[i];
        count *= 10;
    }
    count %= size;
    return count;
}

hash *newHash(int size) {
    hash *h = calloc(1, sizeof(hash));
    h->size = size;
    h->head = calloc(size, sizeof(hash *));
    h->func = dummyHash;
    return h;
}

int hashSet(hash *h, char *key, int len, resp *value, int expires) {
    int num = h->func(key, len, h->size);
    bucket pre;
    bucket *b = h->head[num];
    if (!h->head[num]) {
        h->head[num] = calloc(1, sizeof(bucket));
        b = h->head[num];
    } else {
        while (b && b->next) {
            b = b->next;
        }
        b->next = calloc(1, sizeof(bucket));
        b = b->next;
    }
    b->key = key;
    b->value = value;
    b->len = len;
    b->expires = expires;
    return 1;
}


bucket * __hashGetOrDelete(hash *h, char *key, int len, _Bool del) {
    int num = h->func(key, len, h->size);
    bucket **b = &h->head[num];
    while (*b) {
        if ((*b)->len == len && strncmp(key, (*b)->key, len) == 0) {
            if (del) {
                if ((*b)->next) {
                    *b = (*b)->next;
                } else {
                    *b = NULL;
                }
            }
            return *b;
        }
        b = &(*b)->next;
    }
    return NULL;
}

resp *hashGet(hash *h, char *key, int len) {
    bucket *b = __hashGetOrDelete(h, key, len, 0);
    if (b) {
        return b->value;
    } else {
        return NULL;
    }
}

_Bool hashDel(hash *h, char *key, int len) {
    bucket *b = __hashGetOrDelete(h, key, len, 1);
    if (b) {
        free(b);
        return 1;
    } else {
        return 0;
    }
}
