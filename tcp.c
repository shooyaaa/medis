#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include "defines.h"



int poll(net *n) {
    client *pre = (client*) malloc(sizeof(client));
    client *p;
    pre->next = n->clientPool;
    p = pre;
    int count = 0;
    while (p && p->next != NULL) {
        int bytes = recv(p->next->fd, p->next->readBuf, MAX_BUFFER_SIZE, 0);
        if (bytes > 0) {
            //printf("Read client (%d) : (%s)\n", p->next->fd, p->next->readBuf);
            p->next->rsize = bytes;
            signalList *sl = calloc(1, sizeof(signalList));
            sl->c = p->next;
            addToLinkList(&n->readSignal, sl);
        } else if (bytes == 0) {
            //printf("Client offline (%d) : (%s)\n", p->next->fd, p->next->readBuf);
            p->next->state = STATE_ERROR;
            freeClient(p->next, n);
            p->next = p->next->next;
        }
        p = p->next;
    }
    n->clientPool = pre->next;
    return count;
}
