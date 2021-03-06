#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "defines.h"
#include "schedule.h"
#include "resp.h"

net *server;

void handler(int sig) {
    printf("\nExiting ...\n");
    close(server->fd);
    exit(0);
}

void closeSockets(void *p) {
    net *n = (net *)p;
    signalList *q = n->closedClient;
    int count = 0;
    while (q) {
        close(q->c->fd);
        q = q->next;
        count ++;
    }
    n->closedClient = NULL;
    if (count) {
        printf("Close sockets %d\n", count);
    }
}

schedList *scheduledWork(net *n) {
    sched *onlineCheck = initSched(50000, -1, (void *)n, countClients);
    schedList *sl = (schedList *) calloc(1, sizeof(schedList));
    sl->s = onlineCheck;

    sched *cs = initSched(5000, -1, (void *)n, closeSockets);
    schedList *next = (schedList *) calloc(1, sizeof(schedList));
    next->s = cs;
    sl->next = next;
    return sl;
}


void initCommandTable(net *n) {
    command ping = {
        .name = "PING",
        .handler = pingCommand
    };
    command config = {
        .name = "CONFIG",
        .handler = configCommand
    };
    command set = {
        .name = "SET",
        .handler = setCommand
    };
    command get = {
        .name = "GET",
        .handler = getCommand
    };
    command del = {
        .name = "DEL",
        .handler = delCommand
    };

    n->table[0] = ping;
    n->table[1] = config;
    n->table[2] = set;
    n->table[3] = get;
    n->table[4] = del;
}

int main(int argc, char **argv) {
    server = (net *) calloc(1, sizeof(net));
    signal(SIGINT, handler);
    printf("Medis 1.0 start\n");
    int serverFd = createSocket(8888);
    if (serverFd < 1) {
        FATAL("Failed to create socket");
    }
    initCommandTable(server);
    server->fd = serverFd;
    server->globalHash = newHash(1024);
    struct timespec rem, ts = {
        .tv_nsec = 100
    };
    schedList *sl = scheduledWork(server);
    while (poll(server) > -1) {
        handleSignal(server);
        while (acceptClient(server) == 1) {
        }
        loopSchedList(sl, clock());
        //validClients(server);
        //nanosleep(&ts, &rem);
    }
    printf("Stoped!");
}
