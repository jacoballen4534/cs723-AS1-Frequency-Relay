#include "system.h"
#include "altera_avalon_pio_regs.h"

unsigned int freq = 0;
int newData = 0;

void process_frequency(void *pvParameters)
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

void freq_isr()
{
	freq = IORD(FREQUENCY_ANALYSER_BASE, 0);
	newData = 1;
}
