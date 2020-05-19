#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "defines.h"

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
    if (listen(fd, 3) < 0) {
        printf("Failed to listen socket (%s)", strerror(errno));
        return -4;
    }
    return fd;
}

int acceptClient(net *n) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int clientFd = accept(n->fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (clientFd > 0) {
        int ret = fcntl(clientFd, F_SETFL, fcntl(clientFd, F_GETFL, 0) | O_NONBLOCK);
        client *c = (client*)calloc(1, sizeof(client));
        c->fd = clientFd;
        c->readBuf = (char*)calloc(1, sizeof(MAX_BUFFER_SIZE));
        c->writeBuf = (char*)calloc(1, sizeof(MAX_BUFFER_SIZE));
        c->port = ntohs(address.sin_port);
        printf("New client arrived (%d)\n", c->port);
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
    client *p = (client*) malloc(sizeof(client));
    p->next = n->clientPool;
    int count = 0;
    while (p->next != NULL) {
        int bytes = recv(p->next->fd, p->next->readBuf, MAX_BUFFER_SIZE, 0);
        if (bytes == 0) {
            //printf("Client closed (%d) (%s)\n", p->next->fd, strerror(errno));
            close(p->next->fd);
            if (p->next == n->clientPool) {
                n->clientPool = p->next->next;
            }
            p->next = p->next->next;
            count ++;
        } else {
            p = p->next;
        }
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
        char *name = codec(p->c);
        for (int i = 0; i < 10; i ++) {
            if (strncmp(n->table[i].name, name, strlen(n->table[i].name)) == 0) {
                n->table[i].handler(p->c);
                count ++;
                removeFromLinkList(&n->readSignal, p);
                break;
            }
        }
        p = p->next;
    }
    return count;
}

char *codec(client *c) {
    char *name = (char *)calloc(1, 20);
    int status = 0;
    int p = 0;
    for (int i = 0; i < c->rsize; i++) {
        if (c->readBuf[i] == ' ') {
            if (status == 0) {
                status = 1;
            } else if (status == 2) {
                c->readBuf[p ++] = '\0';
                status = 1;
            }
        } else {
            if (status == 0) {
                name[i] = c->readBuf[i];
            } else if (status == 1){
                status = 2;
                c->readBuf[p ++] = c->readBuf[i];
            } else if (status == 2) {
                c->readBuf[p ++] = c->readBuf[i];
            }
        }
    }
    return name;
}

void pingCommand(client *c) {
    int ret = send(c->fd, "+Pong\r\n", 7, 0);
    printf("ping command (%d)\n", ret);
}
