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
        printf("New client arrived (%d)\n", clientFd);
        client *c = (client*)malloc(sizeof(client));
        c->fd = clientFd;
        c->readBuf = (char*)malloc(sizeof(MAX_BUFFER_SIZE));
        c->writeBuf = (char*)malloc(sizeof(MAX_BUFFER_SIZE));
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

int countClients(net *n) {
    int count = 0;
    client *p = n->clientPool;
    while (p) {
        p = p->next;
        count ++;
    }
    return count;
}

int validClients(net *n) {
    client *p = (client*) malloc(sizeof(client));
    p->next = n->clientPool;
    int count = 0;
    while (p->next) {
        int bytes = read(p->next->fd, p->next->readBuf, MAX_BUFFER_SIZE);
        if (bytes == -1) {
            printf("Client closed (%d)", p->fd);
            p->next = p->next->next;
            if (p->next == n->clientPool) {
                n->clientPool = p->next;
            }
            count ++;
        }
        p = p->next;
    }
    return count;
}
