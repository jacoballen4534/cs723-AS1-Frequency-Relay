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

#define NUM_LOADS 8

#define NEW_FREQ_Q_LENGTH 20
extern QueueHandle_t newFreqQ;

#define LOAD_CONTROL_NOTIFY_Q_LENGTH 30
extern QueueHandle_t loadControlNotifyQ;

#define FREQ_DISPLAY_Q_LENGTH 30
extern QueueHandle_t freqDisplayQ;

#define FREQ_DATA_Q_LENGTH 30
extern QueueHandle_t freqDataQ;

#define SHED_RECONNECT_Q_LENGTH 30
extern QueueHandle_t shedReconnectQ;

typedef struct ShedRequest
{
	bool isShed;
	TickType_t timestamp;
} ShedRequest;

extern float firstShedLatency;
extern float minShedLatency;
extern float maxShedLatency;
extern float avgShedLatency;

#define INPUT_Q_LENGTH 30
extern QueueHandle_t inputQ;

#define WALL_SWITCH_NOTIFICATION 3
#define USER_INPUT_NOTIFICATION 1
#define LOAD_SHEDDER_NOTIFICATION 2

#define PUSH_BUTTON_SPECIAL_VALUE -100



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

extern SemaphoreHandle_t xThreshMutex;
extern double freqThresh;
extern double rocThresh;

#define USER_INPUT_BUFFER_LENGTH 5
#define USER_INPUT_BUFFER_BLOCK_TIME 10
#define USER_INPUT_NOTIFY_LOAD_CONTROL_BLOCK_TIME 5
extern SemaphoreHandle_t xUserInputBufferMutex;
extern uint16_t userInputBufferIndex;
extern char userInputBuffer[USER_INPUT_BUFFER_LENGTH + 1]; // Allow a /0 to be put on the end
extern bool newUserInputValue;								// Only update the LCD on new values to prevent flickering.
extern UpdateType updateType;								// Indicate what value is being updated

extern SemaphoreHandle_t xIsMaintenanceMutex;
extern bool isMaintenance;

#endif /* SHARED_VARS_H */
