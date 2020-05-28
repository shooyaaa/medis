#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "defines.h"
#include "resp.h"

int createSocket(int port) {
    int fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == 0)
    {
        printf("Failed to create socket (%s)", strerror(errno));
        return -1;
    }
    int trueOne = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &trueOne, sizeof(int));

    int ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    if (ret == -1) {
        printf("Failed to set non-blocking socket");
        return -2;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("Failed to bind socket (%s)", strerror(errno));
        return -3;
    }
    if (listen(fd, 10240) < 0) {
        printf("Failed to listen socket (%s)", strerror(errno));
        return -4;
    }
    return fd;
}

void freeClient(client *c, net *n) {
    signalList *sl = calloc(1, sizeof(signalList));
    sl->c = c;
    addToLinkList(&n->closedClient, sl);
}

int acceptClient(net *n) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int clientFd = accept(n->fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (clientFd > 0) {
        int ret = fcntl(clientFd, F_SETFL, fcntl(clientFd, F_GETFL, 0) | O_NONBLOCK);
        client *c = (client*)calloc(1, sizeof(client));
        c->fd = clientFd;
        c->state = STATE_OK;
        c->readBuf = (char*)calloc(1, sizeof(MAX_BUFFER_SIZE));
        c->writeBuf = (char*)calloc(1, sizeof(MAX_BUFFER_SIZE));
        c->port = ntohs(address.sin_port);
        printf("New client arrived (%d)\n", c->port);
        countClients(n);
        c->next = NULL;
        if (!n->clientPool) {
            n->clientPool = c;
        } else {
            client *p = n->clientPool;
            while (p->next) {
                p = p->next;
            }
            p->next = c;
        }
        return 1;
    }
    return 0;
}

void countClients(void *p) {
    net *n = (net *)p;
    int count = 0;
    client *q = n->clientPool;
    while (q) {
        printf("%d", q->port);
        q = q->next;
        if (q) {
            printf("->");
        }
        count ++;
    }
    printf(":Online users (%d)\n", count);
}

int validClients(net *n) {
    client *p, *pre = (client*) malloc(sizeof(client));
    pre->next = n->clientPool;
    p = pre;
    int count = 0;
    while (p && p->next != NULL) {
        int ret = 1;
        if (ret == 0) {
            //printf("Client closed (%d) (%s)\n", p->next->fd, strerror(errno));
            close(p->next->fd);
            p->next = p->next->next;
            count ++;
        }
        p = p->next;
    }
    return count;
}

void addToLinkList(signalList **head, signalList *item) {
    signalList prev;
    prev.next = *head;
    signalList *p = &prev;
    while (p->next) {
        p = p->next;
    }
    p->next = item;
    *head = prev.next;
}

void removeFromLinkList(signalList **head, signalList *item) {
    signalList prev;
    prev.next = *head;
    signalList *p = &prev;
    while (p->next) {
        if (p->next == item) {
            p->next = p->next->next;
        } else {
            p = p->next;
        }
    }
    *head = prev.next;
}

int handleSignal(net *n) {
    signalList *p = n->readSignal;
    int count = 0;
    while (p) {
        resp *rp = codec(p->c);
        dmString *dm = calloc(1, sizeof(dmString));
        while (rp) {
            char *name = rp->s;
            if (IS_ARRAY(rp)) {
                name = rp->arr[0]->s;
            }
            for (int i = 0; i < 10; i ++) {
                int len = strlen(n->table[i].name);
                if (len > 0 && strncmp(n->table[i].name, name, len) == 0) {
                    resp *ret = n->table[i].handler(rp);
                    serialize(ret, dm);
                    count ++;
                    break;
                }
            }
            rp = rp->next;
        }
        send(p->c->fd, dm->str, dm->size, 0);
        removeFromLinkList(&n->readSignal, p);
        p = p->next;
    }
    return count;
}

resp *codec(client *c) {
    resp *rp = calloc(1, sizeof(resp));
    resp *p = rp;
    int start = 0;
    while ((start = parse(c->readBuf, start, c->rsize, p)) < c->rsize) {
        p->next = calloc(1, sizeof(resp));
        p = p->next;
    }
    dmString *dm = calloc(1, sizeof(dmString));
    serialize(rp, dm);
    return rp;
}

resp *pingCommand(resp *rp) {
    resp *ret = calloc(1, sizeof(resp));
    ret->type = RESP_STRING;
    ret->s = "PONG";
    ret->size = 5;
    return ret;
}

resp *configCommand(resp *rp) {
    resp *ret = calloc(1, sizeof(resp));
    printf("Config command\n");
    char *response;
    ret->type = RESP_ARRAY;
    ret->arr = calloc(2, sizeof(resp*));
    ret->arr[0] = rp->arr[2];
    ret->arr[1] = calloc(1, sizeof(resp));
    if (strncmp(rp->arr[2]->s, "save", 4) == 0) {
        response = "900 1 300 10 60 10000";
    } else if (strncmp(rp->arr[2]->s, "appendonly", 10) == 0) {
        response = "no";
    }
    ret->size = 2;
    ret->arr[1]->type = RESP_BULK;
    ret->arr[1]->s = response;
    ret->arr[1]->size = strlen(response);
    return ret;
}
