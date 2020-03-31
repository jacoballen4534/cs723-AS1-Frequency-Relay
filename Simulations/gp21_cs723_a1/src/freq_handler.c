#include <stdio.h>

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "taskMacros.h"


#define FREQUENCY_ANALYSER_SIMULATED_DELAY 20
QueueHandle_t frequencyValueQueue;

// Dummy frequency data to use for simulations
#define NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES 10
unsigned int dummy_frequency_data[NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES] = {320,321,322,318,315,313,310,318,320,325};
int frequency_data_index = 0;

// Forward Declarations of task helper
void handleTaskCreateError(BaseType_t taskStatus, char* taskName);

// Local Function Prototypes
void mockFrequencyAnalyser(void *pvParameters);
void processFrequency(void *pvParameters);


// This function initialises the freq_handler tasks
int initFrequencyAnalyser(void)
{

	// Creating a queue to store frequency readings
	frequencyValueQueue = xQueueCreate(5, sizeof(unsigned int));
	if (frequencyValueQueue == 0) {
		fputs("Could not create a queue", stderr);
	}

	BaseType_t taskStatus;

	taskStatus = xTaskCreate(mockFrequencyAnalyser, "mockFrequencyAnalyser", TASK_STACKSIZE, NULL, FREQ_ISR_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "mockFrequencyAnalyser");

	taskStatus = xTaskCreate(processFrequency, "processFrequency", TASK_STACKSIZE, NULL, FREQ_ANALYSER_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "processFrequency");
	return 0;
}


void processFrequency(void *pvParameters)
{
	while (1)
	{

		unsigned int newFrequencyReading;
		BaseType_t queueReceiveStatus = xQueueReceive(frequencyValueQueue, (void *)&newFrequencyReading, portMAX_DELAY );
		if (queueReceiveStatus == pdTRUE) {
			printf("%f Hz\n", 16000/(double)newFrequencyReading);
		} else {
			printf("frequencyValueQueue is empty\n");
		}
		fflush(stdout);
	}

	//calculate RoC
}




// Handle freq analyser ISR's
void freqISRHandler()
{
	frequency_data_index = frequency_data_index % NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES;
	unsigned int newFrequencyReadingToAdd = dummy_frequency_data[frequency_data_index];

	BaseType_t queueSendStatus = xQueueSendFromISR(frequencyValueQueue, (void *)&newFrequencyReadingToAdd, NULL);
	if (queueSendStatus == pdTRUE) {
		frequency_data_index++;
		printf("new freq data %u \n", newFrequencyReadingToAdd);
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
		freqISRHandler();
	}
}
