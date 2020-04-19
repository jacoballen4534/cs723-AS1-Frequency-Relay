#ifndef SHARED_VARS_H
#define SHARED_VARS_H

#ifdef __SIMULATION__
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#else
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#endif

typedef enum { false, true } bool;

#define NUM_LOADS 10

#define NEW_FREQ_Q_LENGTH 20
QueueHandle_t newFreqQ;

#define LOAD_CONTROL_NOTIFY_Q_LENGTH 30
QueueHandle_t loadControlNotifyQ;

#define FREQ_DISPLAY_Q_LENGTH 30
QueueHandle_t freqDisplayQ;

#define FREQ_DATA_Q_LENGTH 30
QueueHandle_t freqDataQ;	

#define SHED_RECONNECT_Q_LENGTH 30
QueueHandle_t shedReconnectQ;

#define WALL_SWITCH_NOTIFICATION 1
#define USER_INPUT_NOTIFICATION 2
#define LOAD_SHEDDER_NOTIFICATION 3

typedef struct FreqReading
{
    double freq;
	double RoC;
	TickType_t timestamp;
} FreqReading;

typedef struct AnalyserReading
{
    unsigned int count;
	TickType_t timestamp;
} AnalyserReading;

#define DISPLAY_BUFFER_LENGTH 50

SemaphoreHandle_t xThreshMutex;
double freqThresh;
double rocThresh;
#endif /* SHARED_VARS_H */
