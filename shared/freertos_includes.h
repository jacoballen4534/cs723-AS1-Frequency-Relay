#ifndef FREERTOS_INCLUDES_H
#define FREERTOS_INCLUDES_H

#ifdef __SIMULATION__
#include "mockIO.h"
#include "mockSystem.h"
// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#else
#include "io.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"
// Scheduler includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#endif

// Forward Declarations of task helper
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

#endif
