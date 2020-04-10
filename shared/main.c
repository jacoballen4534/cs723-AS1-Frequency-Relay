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

// declare local functions
void initSharedVars(void);

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

	// Starting the scheduler
	vTaskStartScheduler();
	return 0;
}

void initSharedVars(void)
{
	// Creating a queue to buffer frequency readings.
	newFreqQ = xQueueCreate(NEW_FREQ_Q_LENGTH, sizeof(unsigned int));

	if (newFreqQ == 0)
	{
		fputs("Could not create newFreqQ queue", stderr);
		exit(1);
	}

	loadControllNotifyQ = xQueueCreate(LOAD_CONTROLL_NOTIFY_Q_LENGTH, sizeof(unsigned int));
	if (loadControllNotifyQ == 0)
	{
		fputs("Could not create loadControllNotifyQ queue", stderr);
		exit(1);
	}
}
