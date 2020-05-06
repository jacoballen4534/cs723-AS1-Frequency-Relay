#ifndef LOAD_CONTROL_H
#define LOAD_CONTROL_H
#include "vars.h"

extern SemaphoreHandle_t xLatencyMutex; //FIXME: Implement
extern float firstShedLatency;
extern float minShedLatency;
extern float maxShedLatency;
extern float avgShedLatency;
extern bool newLatency;

//feedback from loadControl so we know when to enter 'idle'
extern bool allConnected; //fixme: mutex guard

#endif
