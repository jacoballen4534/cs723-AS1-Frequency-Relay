#include "mockIsrHandlers.h"
#include "mockIO.h"
#include "mockSystem.h"

#define NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES 10

unsigned int IORD(int base, int regnum)
{
	static unsigned int dummy_frequency_data[NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES] = {320, 321, 322, 318, 315, 313, 310, 318, 320, 325};
	static int frequency_data_index = 0;

	// Mock frequency analyser IO
	if (base == FREQUENCY_ANALYSER_BASE)
	{
		unsigned int newReadingToReturn = dummy_frequency_data[frequency_data_index];
		frequency_data_index++;
		frequency_data_index = frequency_data_index % NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES;
		return newReadingToReturn;
	}

	return 0;
}

// register ISR's for mock IO
int alt_irq_register(alt_u32 id, void *context, void (*handler)(void *, alt_u32))
{

	if (id == FREQUENCY_ANALYSER_IRQ) // registering frequency analyser ISR handler
	{
		freq_analyser_isr_handler = handler;
	}
	else if (id == PS2_IRQ) // registering keyboard ISR handler
	{
		keyboard_isr_handler = handler;
		keyboard_context = context;
	}

	return 0;
}

void IOWR_8DIRECT(int base, int offset, int data) {}
void IOWR_16DIRECT(int base, int offset, int data) {}
void IOWR_32DIRECT(int base, int offset, int data) {}
