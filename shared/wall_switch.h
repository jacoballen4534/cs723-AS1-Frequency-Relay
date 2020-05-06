#ifndef WALL_SWITCH_H
#define WALL_SWITCH_H
#include "vars.h"

extern uint8_t switchVal[NUM_LOADS];
extern SemaphoreHandle_t xSwitchMutex;

#endif
