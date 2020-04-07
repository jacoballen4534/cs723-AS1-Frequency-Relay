#ifndef TASK_HELPERS_H
#define TASK_HELPERS_H

#ifdef __SIMULATION__
#include "FreeRTOS.h"
#else
#include "freertos/FreeRTOS.h"
#endif

void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

#endif /* TASK_HELPERS_H */
