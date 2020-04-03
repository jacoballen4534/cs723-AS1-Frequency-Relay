#include "system.h"
#include "altera_avalon_pio_regs.h"

//receives counts from frequency analyser and converts it into a frequency, and a running rate-of-change
//isr writes counts to queue, task reads and does conversions
//communicate frequencies and RoCs to load shedder over shared memory (because history not important, only latest value)
//communicate frequencies and RoCs to display task over shared memory
//2-deep FIFO for current and last value

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
