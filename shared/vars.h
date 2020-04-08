#ifndef SHARED_VARS_H
#define SHARED_VARS_H

#ifdef __SIMULATION__
#include "FreeRTOS.h"
#include "queue.h"
#else
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#endif

#define NUM_SWITCHES 18
QueueHandle_t newFreqQ;

#endif /* SHARED_VARS_H */
