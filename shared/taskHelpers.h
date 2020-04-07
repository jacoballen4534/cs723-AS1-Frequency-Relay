#ifndef TASK_HELPERS_H
#define TASK_HELPERS_H
#include "FreeRTOS.h"

void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

#endif /* TASK_HELPERS_H */
