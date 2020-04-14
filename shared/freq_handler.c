//receives counts from frequency analyser and converts it into a frequency, and a running rate-of-change
//isr writes counts to queue, task reads and does conversions
//communicate frequencies and RoCs to load shedder over shared memory (because history not important, only latest value)
//communicate frequencies and RoCs to display task over shared memory
//2-deep FIFO for current and last value

#include <stdio.h>
#include "taskMacros.h"
#include "vars.h"
#include "freertos_includes.h"

#define SAMPLING_FREQUENCY 16000.0

// Local Function Prototypes
void processFrequency(void *pvParameters);
void freq_isr();

// This function initialises the freq_handler tasks
int initFrequencyAnalyser(void)
{
	// Register freq_isr
	alt_irq_register(FREQUENCY_ANALYSER_IRQ, 0, freq_isr);

	// Start processFrequency task
	BaseType_t taskStatus = xTaskCreate(processFrequency, "processFrequency", TASK_STACKSIZE, NULL, FREQ_ANALYSER_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "processFrequency");

	return 0;
}

void processFrequency(void *pvParameters)
{
	double latestFrequencySample = 0;
	double previousFrequencySample = 0;
	AnalyserReading ADCSamples;
	double roc = 0;

	while (1)
	{

		BaseType_t queueReceiveStatus = xQueueReceive(newFreqQ, (void *)&ADCSamples, portMAX_DELAY);
		if (queueReceiveStatus > 0)
		{
			latestFrequencySample = SAMPLING_FREQUENCY / (double)ADCSamples.count;
		}
		else
		{
			// Something went wrong
			latestFrequencySample = 0;
		}

		

		//calculate RoC
		roc = (latestFrequencySample - previousFrequencySample) * 2.0 * latestFrequencySample * previousFrequencySample / (latestFrequencySample + previousFrequencySample);
		previousFrequencySample = latestFrequencySample;

		FreqReading fr = {latestFrequencySample, roc, ADCSamples.timestamp};
		xQueueSend(freqDisplayQ, (void *)&fr, portMAX_DELAY);
		xQueueSend(freqDataQ, (void *)&fr, 0); //FIXME

		//printf("%f Hz, ", fr.freq);

		//printf("RoC %f, ", fr.RoC);
		//printf("Tick count: %u\r\n", fr.timestamp);
		fflush(stdout);
	}
}

void freq_isr()
{
	unsigned int newReading = IORD(FREQUENCY_ANALYSER_BASE, 0);
	TickType_t timestamp = xTaskGetTickCountFromISR();
	AnalyserReading ar = {newReading, timestamp};
	
	BaseType_t queueSendStatus = xQueueSendFromISR(newFreqQ, (void *)&ar, NULL);

	if (queueSendStatus == pdTRUE)
	{
		printf("new number_of_adc_samples %u \n", newReading);
	}
	else
	{
		printf("newFreqQ is full \n");
	}
	fflush(stdout);
}
