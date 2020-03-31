#include <stdio.h>

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "taskMacros.h"
#include "sharedVars.h"

#define FREQUENCY_ANALYSER_SIMULATED_DELAY 20
#define SAMPLING_FREQUENCY 16000.0

// Dummy frequency data to use for simulations
#define NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES 10

// Forward Declarations of task helper
void handleTaskCreateError(BaseType_t taskStatus, char* taskName);

// Local Function Prototypes
void mockFrequencyAnalyser(void *pvParameters);


// This function initialises the frequency relay tasks
int initFrequencyRelay(void)
{
	BaseType_t taskStatus = xTaskCreate(mockFrequencyAnalyser, "mockFrequencyAnalyser", TASK_STACKSIZE, NULL, FREQ_ISR_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "mockFrequencyAnalyser");
	return 0;
}


void frequencyRelayISRHandler()
{
	static unsigned int dummy_frequency_data[NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES] = {320,321,322,318,315,313,310,318,320,325};
	static int frequency_data_index = 0;

	double newFrequencyReadingToAdd = SAMPLING_FREQUENCY / (double)dummy_frequency_data[frequency_data_index];

	BaseType_t queueSendStatus = xQueueSendFromISR(frequencyValueQueue, (void *)&newFrequencyReadingToAdd, NULL);
	if (queueSendStatus == pdTRUE) {
		frequency_data_index++;
		frequency_data_index = frequency_data_index % NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES;

		printf("new frequency data %f \n", newFrequencyReadingToAdd);
	} else {
		printf("frequencyValueQueue is full \n");
	}
	fflush(stdout);
}


// Simulate the hardware interrupts from the frequency analyser
void mockFrequencyAnalyser(void *pvParameters)
{
	while(1)
	{
		vTaskDelay(FREQUENCY_ANALYSER_SIMULATED_DELAY);
		frequencyRelayISRHandler();
	}
}
