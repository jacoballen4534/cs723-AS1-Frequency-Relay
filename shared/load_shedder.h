#ifndef LOAD_SHEDDER_H
#define LOAD_SHEDDER_H
#include "vars.h"

extern SemaphoreHandle_t xThreshMutex;
extern double freqThresh;
extern double rocThresh;

extern SemaphoreHandle_t xIsMaintenanceMutex;
extern bool isMaintenance;
extern SemaphoreHandle_t xIsManagingMutex;
extern bool isManaging;

#endif
