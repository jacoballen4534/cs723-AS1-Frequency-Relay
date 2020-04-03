#include <stdio.h>

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "taskMacros.h"
#include "sharedVars.h"

// Forward Declarations of task helper
void handleTaskCreateError(BaseType_t taskStatus, char* taskName);

// Local Function Prototypes
void processFrequency(void *pvParameters);


// This function initialises the freq_handler tasks
int initFrequencyAnalyser(void)
{
	BaseType_t taskStatus = xTaskCreate(processFrequency, "processFrequency", TASK_STACKSIZE, NULL, FREQ_ANALYSER_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "processFrequency");
	return 0;
}


void processFrequency(void *pvParameters)
{
	double latestFrequencySample = 0;
	double previousFrequencySample = 0;
	double roc = 0;

	while (1)
	{

		BaseType_t queueReceiveStatus = xQueueReceive(frequencyValueQueue, (void *)&latestFrequencySample, portMAX_DELAY );
		if (queueReceiveStatus == pdTRUE) {
			printf("%f Hz\n", latestFrequencySample);
		} else {
			printf("frequencyValueQueue is empty\n");
		}

		//calculate RoC
		roc = (latestFrequencySample - previousFrequencySample) * 2.0 * latestFrequencySample * previousFrequencySample / (latestFrequencySample + previousFrequencySample);
		previousFrequencySample = latestFrequencySample;
		printf("RoC %f\n\n", roc);

		fflush(stdout);
	}
}
