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
#include <stdlib.h>
#include "taskMacros.h"
#include "freertos_includes.h"
#include "vars.h"

#ifdef __SIMULATION__
#include "mockKeyboard.h"
#else
#include "altera_up_avalon_ps2.h"
#include "altera_up_ps2_keyboard.h"
#endif

#define USER_INPUT_BUFFER_BLOCK_TIME 10
#define PUSH_BUTTON_SPECIAL_VALUE 250
#define USER_INPUT_NOTIFY_LOAD_CONTROL_BLOCK_TIME 5
const char pushButtonSpecialValue = (char)250;
const uint32_t userInputNotificationValue = USER_INPUT_NOTIFICATION;

// Forward declare
void shutDown(void);
// KeyboardISR re-usable variables
char ascii;
int status = 0;
unsigned char key = 0;
KB_CODE_TYPE decode_mode;

// Local Function Prototypes
void keyboard_isr(void *context, alt_u32 id);
void button_isr(void *context, alt_u32 id);
void vUserInputTask(void *pvParameters);

uint16_t userInputBufferIndex;
char userInputBuffer[USER_INPUT_BUFFER_LENGTH + 1] = {0}; // Allow a /0 to be put on the end
bool newUserInputValue;								// Only update the LCD on new values to prevent flickering.
UpdateType updateType;								// Indicate what value is being updated
QueueHandle_t inputQ;
SemaphoreHandle_t xUserInputBufferMutex;

// This function initialises the process that captures keyboard inputs
int initUserInput(void)
{
	alt_up_ps2_dev *keyboard_device = alt_up_ps2_open_dev(PS2_NAME);
	if (keyboard_device == NULL)
	{
		fputs("can't find PS/2 device\n", stderr);
		shutDown();
	}
	alt_up_ps2_clear_fifo(keyboard_device);
	keyboard_device->timeout = 2000000;
	printf("Device type:%d\n", keyboard_device->device_type);
	//alt_up_ps2_init(keyboard_device);
	keyboard_device->device_type = 1;
	unsigned char byte1;
	while(alt_up_ps2_read_data_byte(keyboard_device, &byte1)!=0);
	printf("Device type:%d\n", keyboard_device->device_type);

#ifdef __SIMULATION__
	initMockKeyboard(keyboard_device);
#endif

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

	// Start userInputTask task
	BaseType_t taskStatus = xTaskCreate(vUserInputTask, "vUserInputTask", TASK_STACKSIZE, NULL, USER_INPUT_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "vUserInputTask");

	return 0;
}

bool readyToReceive = false;
void keyboard_isr(void *context, alt_u32 id)
{
	//because this ISR is triggered three times per keypress (MAKE, BREAK, MAKE), decode_scancode does not work as expected
	//we will skip every other valid MAKE to account for this.

	status = decode_scancode(context, &decode_mode, &key, &ascii);
	//translate backspace and enter to ascii since this isn't handled above
	if(key == 0x5A) ascii = '\n';
	else if (key == 0x66) ascii = 8;
	printf("Decode mode: %d\n, key: %d\n", decode_mode, key);
	if(decode_mode == KB_ASCII_MAKE_CODE || decode_mode == KB_BINARY_MAKE_CODE)
	{
		if(readyToReceive)
		{
			readyToReceive = false;
			BaseType_t queueSendStatus = xQueueSendFromISR(inputQ, (void *)&ascii, NULL);
			if (queueSendStatus != pdTRUE)
			{
				printf("inputQ is full (keyboard isr)\n");
			}
		}
		else readyToReceive = true;
	}
}

void button_isr(void *context, alt_u32 id)
{
	char pushButtonSpecialValue = PUSH_BUTTON_SPECIAL_VALUE;
	BaseType_t queueSendStatus = xQueueSendFromISR(inputQ, (void *)&pushButtonSpecialValue, NULL);
	if (queueSendStatus != pdTRUE)
	{
		printf("inputQ is full (button isr)\n");
	}

	// clears the edge capture register
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PUSH_BUTTON_BASE, 0x0001);
}

void vUserInputTask(void *pvParameters)
{
	char input;
	bool gotDecimalPoint = false;
	while (1)
	{
		xQueueReceive(inputQ, (void *)&input, portMAX_DELAY);
		printf("Received char: %d\n", input);
		switch (input)
		{
		case PUSH_BUTTON_SPECIAL_VALUE: // Toggle maintainence mode
			xSemaphoreTake(xIsMaintenanceMutex, USER_INPUT_BUFFER_BLOCK_TIME);
			isMaintenance = !isMaintenance;
			printf("isMaintenance updated to ");
			isMaintenance == true ? printf("true\n") : printf("false\n");
			xSemaphoreGive(xIsMaintenanceMutex);
			BaseType_t result = xQueueSend(loadControlNotifyQ, (void *)&userInputNotificationValue, USER_INPUT_NOTIFY_LOAD_CONTROL_BLOCK_TIME);
			if (result == errQUEUE_FULL)
			{
				printf("user input update fail as loadControllNotifyQ is full\n");
			}

			break;
		case 'q': //Quit
		case 'Q':
			shutDown();
			break;
		case (char)8: //Backspace
			if (userInputBufferIndex > 0)
			{
				userInputBufferIndex--;
				if (userInputBuffer[userInputBufferIndex] == '.')
				{
					gotDecimalPoint = false;
				}
				userInputBuffer[userInputBufferIndex] = 0;
				newUserInputValue = true;
			}
			else
			{
				printf("nothing left to delete\n");
			}
			break;

		case 'f':
		case 'F':
			updateType = Frequency;
			newUserInputValue = true;
			userInputBufferIndex = 0;
			userInputBuffer[userInputBufferIndex] = 0;
			break;

		case 'r':
		case 'R':
			updateType = Roc;
			newUserInputValue = true;
			userInputBufferIndex = 0;
			userInputBuffer[userInputBufferIndex] = 0;
			break;

		case (char)10: // Enter
		case (char)13:
			if (userInputBufferIndex == 0)
			{
				break;
			}
			xSemaphoreTake(xUserInputBufferMutex, USER_INPUT_BUFFER_BLOCK_TIME);
			float newValue = atof(userInputBuffer);
			userInputBufferIndex = 0;
			xSemaphoreGive(xUserInputBufferMutex);

			xSemaphoreTake(xThreshMutex, portMAX_DELAY);
			if (updateType == Frequency)
			{
				freqThresh = newValue;
			}
			else
			{
				rocThresh = newValue;
			}
			xSemaphoreGive(xThreshMutex);

			gotDecimalPoint = false;
			newUserInputValue = true;
			break;
		case '.':
			if (!gotDecimalPoint) // only allow a single decimal point
			{
				xSemaphoreTake(xUserInputBufferMutex, USER_INPUT_BUFFER_BLOCK_TIME);
				userInputBuffer[userInputBufferIndex] = '.';
				userInputBuffer[userInputBufferIndex+1] = 0;
				userInputBufferIndex++;
				newUserInputValue = true;
				gotDecimalPoint = true;
				xSemaphoreGive(xUserInputBufferMutex);
			}
			break;
		default:
			if (userInputBufferIndex < USER_INPUT_BUFFER_LENGTH)
			{
				if ('0' <= input && input <= '9') // Only accept numbers
				{
					xSemaphoreTake(xUserInputBufferMutex, USER_INPUT_BUFFER_BLOCK_TIME);
					userInputBuffer[userInputBufferIndex] = input;
					userInputBuffer[userInputBufferIndex+1] = 0;
					userInputBufferIndex++;
					newUserInputValue = true;
					xSemaphoreGive(xUserInputBufferMutex);
				}
			}
			else
			{
				printf("userInputBuffer is full\n");
			}

			break;
		}
	}
}
