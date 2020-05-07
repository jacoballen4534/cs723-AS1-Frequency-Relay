#include <stdio.h>
#include "vars.h"
#include "user_input.h"
#include "load_shedder.h"

#ifdef __SIMULATION__
// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#else
// Scheduler includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

// Forward Declarations of task initialisers
#ifdef __SIMULATION__
int initMockFrequencyRelay(void);
#endif

void handleQueueCreateError(QueueHandle_t queueStatus, char *queueName);
int initFrequencyAnalyser(void);
int initUserInput(void);
int initWallSwitches(void);
int initLoadControl(void);
int initDisplay(void);
int initLoadShedder(void);

// declare local functions
void initSharedVars(void);
void shutDown(void);

int main(int argc, char *argv[], char *envp[])
{
	initSharedVars();

#ifdef __SIMULATION__
	initMockFrequencyRelay();
#endif

	// Initialise all tasks
	initFrequencyAnalyser();
	initUserInput();
	initWallSwitches();
	initLoadControl();
	initDisplay();
	initLoadShedder();

	// Starting the scheduler
	vTaskStartScheduler();
	return 0;
}

void initSharedVars(void)
{
	// Creating a queue to buffer frequency readings.
	newFreqQ = xQueueCreate(NEW_FREQ_Q_LENGTH, sizeof(AnalyserReading));
	handleQueueCreateError(newFreqQ, "newFreqQ");

	loadControlNotifyQ = xQueueCreate(LOAD_CONTROL_NOTIFY_Q_LENGTH, sizeof(unsigned int));
	handleQueueCreateError(loadControlNotifyQ, "loadControlNotifyQ");

	freqDisplayQ = xQueueCreate(FREQ_DISPLAY_Q_LENGTH, sizeof(FreqReading));
	handleQueueCreateError(freqDisplayQ, "freqDisplayQ");

	freqDataQ = xQueueCreate(FREQ_DATA_Q_LENGTH, sizeof(FreqReading));
	handleQueueCreateError(freqDataQ, "freqDataQ");

	shedReconnectQ = xQueueCreate(SHED_RECONNECT_Q_LENGTH, sizeof(ShedRequest));
	handleQueueCreateError(shedReconnectQ, "shedReconnectQ");

	inputQ = xQueueCreate(INPUT_Q_LENGTH, sizeof(char));
	handleQueueCreateError(inputQ, "inputQ");

	timerQ = xQueueCreate(TIMER_Q_LENGTH, sizeof(uint8_t));
	handleQueueCreateError(timerQ, "timerQ");

	newLoadStatusToDisplayQ = xQueueCreate(MAIL_BOX_LENGTH, sizeof(uint8_t));
	handleQueueCreateError(newLoadStatusToDisplayQ, "newLoadStatusToDisplay");

	newSwitchValToDisplayQ = xQueueCreate(MAIL_BOX_LENGTH, sizeof(uint8_t));
	handleQueueCreateError(newSwitchValToDisplayQ, "newLoadStatusToDisplay");

	newThresholdToDisplayQ = xQueueCreate(MAIL_BOX_LENGTH, sizeof(uint8_t));
	handleQueueCreateError(newThresholdToDisplayQ, "newThresholdToDisplayQ");

	newLatencyToDisplayQ = xQueueCreate(MAIL_BOX_LENGTH, sizeof(uint8_t));
	handleQueueCreateError(newLatencyToDisplayQ, "newLatencyToDisplayQ");

	freqThresh = 49.0;
	rocThresh = 8.0;
	xThreshMutex = xSemaphoreCreateMutex();

	xUserInputBufferMutex = xSemaphoreCreateMutex();
	newUserInputValue = true;
	updateType = Frequency;

	xIsMaintenanceMutex = xSemaphoreCreateMutex();
	isMaintenance = false;
	newIsMaintenanceToDisplayQ = xQueueCreate(MAIL_BOX_LENGTH, sizeof(uint8_t));
	handleQueueCreateError(newIsMaintenanceToDisplayQ, "newIsMaintenanceToDisplayQ");
}

void shutDown(void)
{
	vTaskSuspendAll();
	printf("_q\n");
	fflush(stdin);
#ifdef __SIMULATION__
	exit(1);
#endif
}
