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

#include "mockSystem.h"
#include "mockKeyboard.h"
#include "mockIO.h"

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "taskMacros.h"

// Local Function Prototypes
void keyboard_isr(void* context, alt_u32 id);


// This function initialises the process that captures keyboard inputs
int initUserInput(void)
{
	alt_up_ps2_dev * keyboard_device = alt_up_ps2_open_dev(PS2_NAME);

	if(keyboard_device == NULL){
		fputs("can't find PS/2 device\n", stderr);
		exit(1);
	}

	BaseType_t taskStatus;
	initMockKeyboard(keyboard_device);

	alt_up_ps2_clear_fifo(keyboard_device);

	// register the PS/2 interrupt
	alt_irq_register(PS2_IRQ, keyboard_device, keyboard_isr);

	IOWR_8DIRECT(PS2_BASE,4,1);
	return 0;
}


void keyboard_isr(void* context, alt_u32 id)
{
	char ascii;
	int status = 0;
	unsigned char key = 0;
	KB_CODE_TYPE decode_mode;
	status = decode_scancode(context, &decode_mode , &key , &ascii);
	if ( status == 0 ) //success
	{
		printf ( "New keyboard input %c\n code: %d\n", ascii, (int)ascii);
		fflush(stdout);
	}
}
