/*
 * Will contain logic relating to the keyboard and buttons.
 * Keyboard is used to set the threshold, button used to set maintenance mode
 * Contains ISR for capturing keyboard input, and KeyboardTask for processing inputs
 */

//UserInputTask

//keyToggled = 1

//ISR Keyboard
/*
 * On ISR trigger, add key value to queue
 */

//ISR Button
/*
 * add special character to queue to represent button press
 */

//Task
/*
 * while(true) read from queue
 *    if(special)
 *    	 process_maintenace_button
 *    else
 *    	add to buffer
 *    	if newline character, process buffer
 * automatically yields when no data in queue
 */

//Convert buffer to numerical update values
//Update threshold in LoadShedderTask
//	UserInputTask -> CurrentThreshold -> LoadShedder Task
//Update maintenance mode in LoadControlTask
//  UserInputTask -> KeyToggled -> LoadControlTask

#include <stdio.h>
#include "taskMacros.h"
#include "freertos_includes.h"
#include "vars.h"

#ifdef __SIMULATION__
#include "mockKeyboard.h"
#else
#include "altera_up_avalon_ps2.h"
#include "altera_up_ps2_keyboard.h"
#endif

// KeyboardISR re-usable variables
char ascii;
int status = 0;
unsigned char key = 0;
KB_CODE_TYPE decode_mode;

// Local Function Prototypes
void keyboard_isr(void *context, alt_u32 id);
void button_isr(void *context, alt_u32 id);

// This function initialises the process that captures keyboard inputs
int initUserInput(void)
{
	alt_up_ps2_dev *keyboard_device = alt_up_ps2_open_dev(PS2_NAME);

	if (keyboard_device == NULL)
	{
		fputs("can't find PS/2 device\n", stderr);
		exit(1);
	}

#ifdef __SIMULATION__
	initMockKeyboard(keyboard_device);
#endif

	alt_up_ps2_clear_fifo(keyboard_device);
	// register the PS/2 interrupt
	alt_irq_register(PS2_IRQ, keyboard_device, keyboard_isr);

	IOWR_8DIRECT(PS2_BASE, 4, 1);

	// Setup push button

	// clears the edge capture register. Writing 1 to bit clears pending interrupt for corresponding button.
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE, 0x0001);

	// enable interrupts for all buttons
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PUSH_BUTTON_BASE, 0x0001);

	// register the ISR
	alt_irq_register(PUSH_BUTTON_IRQ, NULL, button_isr);

	return 0;
}

void keyboard_isr(void *context, alt_u32 id)
{
	status = decode_scancode(context, &decode_mode, &key, &ascii);
	if (status == 0) //success
	{
		printf("New keyboard input %c\n", ascii);
		fflush(stdout);
		if (ascii == 'q' || ascii == 'Q')
		{
			printf("_quit\n");
		}
	}
}

void button_isr(void *context, alt_u32 id)
{

	printf("Push button was pressed ISR\n");

	// clears the edge capture register
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE, 0x0001);
}
