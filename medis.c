#include <stdio.h>
#include <time.h>
#include "defines.h"
#include "defines.h"


int main(int argc, char **argv) {
    printf("Medis 1.0 start\n");
    net server = {
        .fd = createSocket(8888)
    };
    struct timespec rem, ts = {
        .tv_nsec = 100
    };
    while (poll(&server) > -1) {
        while (acceptClient(&server) == 1) {
        }
        nanosleep(&ts, &rem);
    }
    printf("Stoped!");
}
