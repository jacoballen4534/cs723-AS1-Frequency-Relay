#ifndef SHARED_VARS_H
#define SHARED_VARS_H

#ifdef __SIMULATION__
#include "FreeRTOS.h"
#include "queue.h"
#else
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#endif

#define NUM_LOADS 10

#define NEW_FREQ_Q_LENGTH 20
QueueHandle_t newFreqQ;

#define LOAD_CONTROLL_NOTIFY_Q_LENGTH 30
QueueHandle_t loadControllNotifyQ;

#define WALL_SWITCH_NOTIFICATION 1
#define USER_INPUT_NOTIFICATION 2
#define LOAD_SHEDDER_NOTIFICATION 3

#endif /* SHARED_VARS_H */
