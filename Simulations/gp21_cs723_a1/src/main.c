#include <stdio.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "vars.h"

// Forward Declarations of task initialisers
#ifdef __SIMULATION__
int initMockFrequencyRelay(void);
#endif

int initFrequencyAnalyser(void);
int initUserInput(void);

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

	// Starting the scheduler
	vTaskStartScheduler();
	return 0;
}

void initSharedVars(void)
{
	// Creating a queue to buffer frequency readings.
	newFreqQ = xQueueCreate(20, sizeof(unsigned int));
	if (newFreqQ == 0)
	{
		fputs("Could not create a queue", stderr);
		exit(1);
	}
}
