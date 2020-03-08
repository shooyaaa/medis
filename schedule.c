#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "schedule.h"

sched *initSched(int interval, int times, void *data, void (*handler)(void *)) {
    sched *s = (sched*) malloc(sizeof(sched));
    s->interval = interval;
    s->times = times;
    s->lastTime = clock();
    s->handler = handler;
    s->data = (void *)malloc(sizeof(void *));
    s->data = data;
    return s;
}

int deleteSched(schedList *sl, sched *s) {
    if (!sl) return 0;
    if (sl->s == s) {
        sl = sl->next;
        return 1;
    }
    schedList *p = sl;
    while (p && p->next) {
        if (p->next->s == s) {
            p->next = p->next->next;
            return 1;
        } else {
            p = p->next;
        }
    }
    return 0;
}

void loopSchedList(schedList *sl, clock_t now) {
    schedList *p = sl;
    int msec;
    while (p) {
        msec = (now - p->s->lastTime) * 1000 / CLOCKS_PER_SEC;
        if (msec > p->s->interval) {
            p->s->count ++;
            p->s->lastTime = now;
            if (p->s->count > p->s->times && p->s->times > 0) {
                deleteSched(sl, p->s);
            } else {
                p->s->handler(p->s->data);
            }
        }
        p = p->next;
    }
}
