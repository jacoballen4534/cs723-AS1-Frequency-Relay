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
#include <conio.h>

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "taskMacros.h"


#define MOCK_KEYBOARD_POLL_DELAY 10
#define PRINT_LETTER_DELAY 10

// Forward Declarations of task helper
void handleTaskCreateError(BaseType_t taskStatus, char* taskName);

// Local Function Prototypes
void mockKeyboardInput(void *pvParameters);

// This function initialises the process that captures keyboard inputs
int initUserInput(void)
{
	BaseType_t taskStatus;

	taskStatus = xTaskCreate(mockKeyboardInput, "mockKeyboardInput", TASK_STACKSIZE, NULL, MOCK_KEYBOARD_ISR_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "mockKeyboardInput");

	return 0;
}


// Simulate the hardware interrupts from keyboard events
void mockKeyboardInput(void *pvParameters)
   {
	char keyPressedAscii = '\0';
   	while(1)
   	{
   		/* Has a key been pressed? */
   		if( _kbhit() != 0 )
   		{
   			keyPressedAscii = _getch();
			printf("Key pressed, %c\n", keyPressedAscii);
			fflush(stdout);
   		}

   	}
}
