#include <stdio.h>
#include <unistd.h>
#include "defines.h"

int poll(net *n) {
    client *p = n->clientPool;
    while (p) {
        int bytes = read(p->fd, p->readBuf, MAX_BUFFER_SIZE);
        if (bytes > 0) {
            printf("Read client (%d) : (%s)\n", p->fd, p->readBuf);
            write(p->fd, "+OK\r\n", 5);
        }
        p = p->next;
    }
    return 0;
}
