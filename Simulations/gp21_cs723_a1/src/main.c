// Standard includes
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// Definition of Task Stacks
#define TASK_STACKSIZE 2048

// Definition of Task Priorities
#define FREQ_ANALYSER_TASK_PRIORITY 1
#define FREQ_ISR_TASK_PRIORITY 5

// Local Function Prototypes
int initFrequencyAnalyser(void);
void handleTaskCreateError(BaseType_t taskStatus, char* taskName);

// Forward Declarations of freq_handler tasks
void mockFreq   freqSampleComplete(void *pvParameters);
void processFrequency(void *pvParameters);


int main(int argc, char* argv[], char* envp[])
{
	initFrequencyAnalyser();

	// Starting the scheduler
	vTaskStartScheduler();

	// In case if the scheduler failed to start.
//	for(;;);

	return 0;
}


// This function initialises the freq_handler tasks
int initFrequencyAnalyser(void)
{
	BaseType_t taskStatus;

	taskStatus = xTaskCreate(freqSampleComplete, "freqSampleComplete", TASK_STACKSIZE, NULL, FREQ_ISR_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "freqSampleComplete");

	taskStatus = xTaskCreate(processFrequency, "processFrequency", TASK_STACKSIZE, NULL, FREQ_ANALYSER_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "processFrequency");
	return 0;
}



// This function handles any errors when creating tasks.
void handleTaskCreateError(BaseType_t taskStatus, char* taskName) {
	if(taskStatus == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
		fprintf(stderr, "Could not allocate required memory for %s", taskName);
		exit(1);
	}
}

