#include <stdio.h>
#include "mockAlteraAvalonPioRegs.h"

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "taskMacros.h"
#include "vars.h"


#define MOCK_FREQUENCY_ANALYSER_DELAY 1000

// Forward Declarations of task helper
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

// Local Function Prototypes
void mockFrequencyAnalyser(void *pvParameters);

// This function initialises the frequency relay tasks
int initMockFrequencyRelay(void)
{
	BaseType_t taskStatus = xTaskCreate(mockFrequencyAnalyser, "mockFrequencyAnalyser", TASK_STACKSIZE, NULL, FREQ_ISR_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "mockFrequencyAnalyser");
	return 0;
}

// Simulate the hardware interrupts from the frequency analyser
void mockFrequencyAnalyser(void *pvParameters)
{
	while (1)
	{
		vTaskDelay(MOCK_FREQUENCY_ANALYSER_DELAY);
		(*freq_analyser_isr_handler)(0, 0);
	}
}

