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
			newData = 0;
		}
	}

	//calculate RoC
}

//REPLACE WITH QUE
int dummy_frequency_data[NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES] = {1,2,3,4,5,6,7,8,9,10};
int frequency_data_index = 0;
const TickType_t frequency_analiser_simulator_delay = 50;

void freqSampleComplete(void *pvParameters)
{
	while(1)
	{

		vTaskDelay(frequency_analiser_simulator_delay);
	}
}

void freqISRHandler()
{
	freq = dummy_frequency_data[frequency_data_index];
	frequency_data_index = ++frequency_data_index % NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES;

	printf("new freq data %u \n", freq);
	fflush(stdout);

	newData = 1;
}
