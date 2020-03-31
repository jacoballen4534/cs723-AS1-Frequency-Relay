#include <stdio.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "sharedVars.h"

// Forward Declarations of task initialisers
int initFrequencyRelay(void);
int initFrequencyAnalyser(void);

// declare local functions
void initSharedVars(void);


int main(int argc, char* argv[], char* envp[])
{
	initSharedVars();

	// Initialise all tasks
	initFrequencyRelay();
	initFrequencyAnalyser();

	// Starting the scheduler
	vTaskStartScheduler();
	return 0;
}

void initSharedVars(void)
{
	// Creating a queue to store frequency readings
	frequencyValueQueue = xQueueCreate(5, sizeof(double));
	if (frequencyValueQueue == 0) {
		fputs("Could not create a queue", stderr);
	}

}
