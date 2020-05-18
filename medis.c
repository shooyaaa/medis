#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "defines.h"
#include "schedule.h"
#include "resp.h"


schedList *scheduledWork(net *n) {
    sched *onlineCheck = initSched(5000, -1, (void *)n, countClients);
    schedList *sl = (schedList *) calloc(1, sizeof(schedList));
    sl->s = onlineCheck;
    return sl;
}

void initCommandTable(net *n) {
    command ping = {
        .name = "PING",
        .handler = pingCommand
    };
    n->table[0] = ping;
}

int main(int argc, char **argv) {
    printf("Medis 1.0 start\n");
    int serverFd = createSocket(8888);
    net *server = (net *) calloc(1, sizeof(net));
    initCommandTable(server);
    server->fd = serverFd;
    struct timespec rem, ts = {
        .tv_nsec = 100
    };
    schedList *sl = scheduledWork(server);
    while (poll(server) > -1) {
        handleSignal(server);
        validClients(server);
        while (acceptClient(server) == 1) {
        }
        loopSchedList(sl, clock());
        nanosleep(&ts, &rem);
    }
    printf("Stoped!");
}
