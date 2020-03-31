#include <stdio.h>

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"

#define NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES 10

unsigned int freq = 0;
int newData = 0;

void processFrequency(void *pvParameters)
{
	while (1)
	{
		if(newData == 1) //REPLACE WITH SEMAPHORE
		{
			printf("%f Hz\n", 16000/(double)freq);
			fflush(stdout);
			newData = 0;
		}
	}

	//calculate RoC
}

//REPLACE WITH QUE
int dummy_frequency_data[NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES] = {320,321,322,318,315,313,310,318,320,325};
int frequency_data_index = 0;
const TickType_t frequency_analiser_simulator_delay = 50;

// Handle freq analyser ISR's
void freqISRHandler()
{
	frequency_data_index = frequency_data_index % NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES;
	freq = dummy_frequency_data[frequency_data_index];
	frequency_data_index++;
	printf("new freq data %u \n", freq);
	fflush(stdout);
	newData = 1;
}


// Simulate the frequency analyser that triggers hardware ISR's
void mockFrequencyAnalyser(void *pvParameters)
{
	while(1)
	{
		vTaskDelay(frequency_analiser_simulator_delay);
		freqISRHandler();
	}
}
