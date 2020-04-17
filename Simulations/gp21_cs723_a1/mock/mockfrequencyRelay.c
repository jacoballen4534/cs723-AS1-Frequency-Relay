#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "taskMacros.h"
#include "vars.h"

#include "mockIsrHandlers.h"

#define MOCK_FREQUENCY_ANALYSER_DELAY 100

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
		vTaskDelay(MOCK_FREQUENCY_ANALYSER_DELAY / portTICK_PERIOD_MS);
		if (freq_analyser_isr_handler)
		{
			(*freq_analyser_isr_handler)(0, 0);
		}
		else
		{
			printf("New freq event. No freq_analyser_isr_handler defined\n");
		}
	}
}
