#include <stdio.h>
#include <stdlib.h>
<<<<<<< HEAD
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
=======
>>>>>>> add redis protocol codec
#include <unistd.h>
#include "defines.h"

void closeSocket(int fd) {
    return;
    printf("Close socket %d\n", fd);
}

int poll(net *n) {
    client *pre = (client*) malloc(sizeof(client));
    client *p;
    pre->next = n->clientPool;
    p = pre;
    int count = 0;
    while (p->next != NULL) {
        int bytes = recv(p->next->fd, p->next->readBuf, MAX_BUFFER_SIZE, 0);
        if (bytes > 0) {
            printf("Read client (%d) : (%s)\n", p->next->fd, p->next->readBuf);
            write(p->next->fd, "+PONG\r\n", 7);
            closeSocket(p->next->fd);
            p->next = p->next->next;
            printf("Read client (%d) : (%s)\n", p->fd, p->readBuf);
            p->rsize = bytes;
            signalList *sl = calloc(1, sizeof(signalList));
            sl->c = p;
            addToLinkList(&n->readSignal, sl);
        }
    }
    return count;
}
