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

typedef enum
{
	false,
	true
} bool;

typedef enum
{
	Frequency,
	Roc
} UpdateType;

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

#define INPUT_Q_LENGTH 30
QueueHandle_t inputQ;

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

#define USER_INPUT_BUFFER_LENGTH 5
SemaphoreHandle_t xUserInputBufferMutex;
uint16_t userInputBufferIndex;
char userInputBuffer[USER_INPUT_BUFFER_LENGTH + 1]; // Allow a /0 to be put on the end
bool newUserInputValue;								// Only update the LCD on new values to prevent flickering.
UpdateType updateType;								// Indicate what value is being updated

SemaphoreHandle_t xIsMaintenanceMutex;
bool isMaintenance;

#endif /* SHARED_VARS_H */
