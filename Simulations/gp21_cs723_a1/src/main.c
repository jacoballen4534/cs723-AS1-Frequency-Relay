// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// Forward Declarations of task initialisers
int initFrequencyAnalyser(void);

int main(int argc, char* argv[], char* envp[])
{
	// Initialise all tasks
	initFrequencyAnalyser();

	// Starting the scheduler
	vTaskStartScheduler();
	return 0;
}

