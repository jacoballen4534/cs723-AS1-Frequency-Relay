#include "mockIO.h"
#include "mockSystem.h"
#include "mockAlteraAvalonPioRegs.h"

#define NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES 10

unsigned int IORD(int BASE, int REGNUM)
{
	static unsigned int dummy_frequency_data[NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES] = {320, 321, 322, 318, 315, 313, 310, 318, 320, 325};
	static int frequency_data_index = 0;

	// Mock frequency analyser IO
	if (BASE == FREQUENCY_ANALYSER_BASE) {
		unsigned int newReadingToReturn = dummy_frequency_data[frequency_data_index];
		frequency_data_index++;
		frequency_data_index = frequency_data_index % NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES;
		return newReadingToReturn;
	}

	return 0;
}

// register ISR's for mock IO
int alt_irq_register( alt_u32 id, void* context, void (*handler)(void*, alt_u32) ){

	// registering frequency analyser ISR handler
	if (id == FREQUENCY_ANALYSER_IRQ) {
		freq_analyser_isr_handler = handler;
	}

	return 0;
}
