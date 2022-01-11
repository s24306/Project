#ifndef BANKINGSYSTEM_TICKCOUNT_H
#define BANKINGSYSTEM_TICKCOUNT_H

#include "time.h"

unsigned long GetTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

#endif //BANKINGSYSTEM_TICKCOUNT_H
