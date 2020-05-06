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

#include "queue_types.h"

//Global FreeRTOS variables and defines
#define MAIL_BOX_LENGTH 1
#define NUM_LOADS 8

#define NEW_FREQ_Q_LENGTH 20
extern QueueHandle_t newFreqQ;

#define FREQ_DISPLAY_Q_LENGTH 30
extern QueueHandle_t freqDisplayQ;

#define FREQ_DATA_Q_LENGTH 30
extern QueueHandle_t freqDataQ;

/**********Load Control**********/
#define SHED_RECONNECT_Q_LENGTH 30
extern QueueHandle_t shedReconnectQ;

#define LOAD_CONTROL_NOTIFY_Q_LENGTH 30
extern QueueHandle_t loadControlNotifyQ;

extern QueueHandle_t newLoadStatusToDisplayQ;
/********************************/

#define INPUT_Q_LENGTH 30
extern QueueHandle_t inputQ;

#define WALL_SWITCH_NOTIFICATION 4
#define WALL_SWITCH_RESET_NOTIFICATION 3
#define USER_INPUT_NOTIFICATION 1
#define LOAD_SHEDDER_NOTIFICATION 2

extern QueueHandle_t newSwitchValToDisplayQ;

#define PUSH_BUTTON_SPECIAL_VALUE -100

#define DISPLAY_BUFFER_LENGTH 50

/**********User Input**********/
#define USER_INPUT_BUFFER_LENGTH 5
#define USER_INPUT_BUFFER_BLOCK_TIME 10
#define USER_INPUT_NOTIFY_LOAD_CONTROL_BLOCK_TIME 5
/******************************/

extern QueueHandle_t newThresholdToDisplayQ;
extern QueueHandle_t newLatencyToDisplayQ;
extern QueueHandle_t newIsMaintenanceToDisplayQ;

#endif /* SHARED_VARS_H */
