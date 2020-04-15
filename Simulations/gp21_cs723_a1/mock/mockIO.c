#include "mockIsrHandlers.h"
#include "mockIO.h"
#include "mockSystem.h"
#include "mockSwitches.h"
#include "mockButtons.h"

#define NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES 120

unsigned int IORD(int base, int regnum)
{
	static unsigned int dummy_frequency_data[NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES] = {
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		325,
		326,
		327,
		328,
		329,
		330,
		331,
		332,
		333,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		334,
		333,
		332,
		331,
		330,
		329,
		328,
		327,
		326,
		325,
	};
	static int frequency_data_index = 0;

	if (base == FREQUENCY_ANALYSER_BASE) // Mock frequency analyser IO
	{
		unsigned int newReadingToReturn = dummy_frequency_data[frequency_data_index];
		frequency_data_index++;
		frequency_data_index = frequency_data_index % NUMBER_OF_DUMMY_FREQUENCY_DATA_SAMPLES;
		return newReadingToReturn;
	}
	else if (base == SLIDE_SWITCH_BASE) // Mock switch IO
	{
		// read switch status variable
		return switchValue;
	}
	else if (base == PUSH_BUTTON_BASE) // Mock button IO. Note not implamented due to only needing 1 button for this project.
	{
		return buttonState;
	}
	return 0;
}

void IOWR(int base, int regnum, unsigned int data)
{
	if (regnum == 3) // IOWR_ALTERA_AVALON_PIO_EDGE_CAP
	{
		if (base == PUSH_BUTTON_BASE) //Clear pending button interrupts
		{
			buttonInterruptPendingMask = data;
		}
	}
	else if (regnum == 2) //IOWR_ALTERA_AVALON_PIO_IRQ_MASK
	{
		if (base == PUSH_BUTTON_BASE) //Setting button interrupt enable mask
		{
			buttonInterruptEnableMask = data;
		}
	}
}

// register ISR's for mock IO
int alt_irq_register(alt_u32 id, void *context, void (*handler)(void *, alt_u32))
{

	if (id == FREQUENCY_ANALYSER_IRQ) // registering frequency analyser ISR handler
	{
		freq_analyser_isr_handler = handler;
		freq_analyser_context = context;
	}
	else if (id == PS2_IRQ) // registering keyboard ISR handler
	{
		keyboard_isr_handler = handler;
		keyboard_context = context;
	}
	else if (id == PUSH_BUTTON_IRQ) // registering button ISR handler
	{
		button_isr_handler = handler;
		button_context = context;
	}

	return 0;
}

void IOWR_8DIRECT(int base, int offset, int data) {}
void IOWR_16DIRECT(int base, int offset, int data) {}
void IOWR_32DIRECT(int base, int offset, int data) {}
