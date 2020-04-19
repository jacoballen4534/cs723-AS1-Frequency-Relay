#include <stdio.h>
#include "vars.h"

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

	if (newFreqQ == 0)
	{
		fputs("Could not create newFreqQ queue", stderr);
		shutDown();
	}

	loadControlNotifyQ = xQueueCreate(LOAD_CONTROL_NOTIFY_Q_LENGTH, sizeof(unsigned int));
	if (loadControlNotifyQ == 0)
	{
		fputs("Could not create loadControlNotifyQ queue", stderr);
		shutDown();
	}

	freqDisplayQ = xQueueCreate(FREQ_DISPLAY_Q_LENGTH, sizeof(FreqReading));
	if (freqDisplayQ == 0)
	{
		fputs("Could not create freqDisplayQ queue", stderr);
		shutDown();
	}

	freqDataQ = xQueueCreate(FREQ_DATA_Q_LENGTH, sizeof(FreqReading));
	if (freqDataQ == 0)
	{
		fputs("Could not create freqDataQ queue", stderr);
		shutDown();
	}

	shedReconnectQ = xQueueCreate(SHED_RECONNECT_Q_LENGTH, sizeof(int));
	if (shedReconnectQ == 0)
	{
		fputs("Could not create shedReconnectQ queue", stderr);
		shutDown();
	}

	inputQ = xQueueCreate(INPUT_Q_LENGTH, sizeof(char));
	if (inputQ == 0)
	{
		fputs("Could not create inputQ queue", stderr);
		shutDown();
	}

	freqThresh = 49.0;
	rocThresh = 8.0;
	xThreshMutex = xSemaphoreCreateMutex();

	xUserInputBufferMutex = xSemaphoreCreateMutex();
	userInputBufferIndex = 0;
	newUserInputValue = true;
	updateType = Frequency;

	xIsMaintenanceMutex = xSemaphoreCreateMutex();
	isMaintenance = false;
}

void shutDown(void)
{
	printf("_quit\n");
	fflush(stdin);
#ifdef __SIMULATION__
	getchar();
	exit(1);
#endif
}