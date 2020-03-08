#ifndef __schedule__
#define __schedule__
#include <time.h>
struct __sched {
    int interval;
    int times;
    int count;
    clock_t lastTime;
    void (*handler)(void *);
    void *data;
};

typedef struct __sched sched;

struct __schedList {
    sched * s;
    struct __schedList * next;
};
typedef struct __schedList schedList;
sched *initSched(int interval, int times, void *data, void (*handler)(void *));
int deleteSched(schedList *sl, sched *s);
void loopSchedList(schedList *sl, clock_t now);
#endif
