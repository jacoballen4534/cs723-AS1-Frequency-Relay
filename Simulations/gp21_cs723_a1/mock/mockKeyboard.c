#include "mockKeyboard.h"
#include "mockSwitches.h"
#include "mockButtons.h"
#include "mockSystem.h"
#include "mockIsrHandlers.h"

#include <string.h>
#include <stdio.h>
#include <conio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "taskMacros.h"

// Forward Declarations of task helper
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

// Forward declare local function
void mockKeyboardInput(void *pvParameters);

// Keys z,x,c,v represent buttons 3,2,1,0 respectively. (Button 3 (left) resets the board)
#define BUTTON_0_KEY 118
#define BUTTON_1_KEY 99
#define BUTTON_2_KEY 120
#define BUTTON_3_KEY 122

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

// Conversion tables for scan codes.
#define SCAN_CODE_NUM 102

char *key_table[SCAN_CODE_NUM] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "`", "-", "=", "\\", "BKSP", "SPACE", "TAB", "CAPS", "L SHFT", "L CTRL", "L GUI", "L ALT", "R SHFT", "R CTRL", "R GUI", "R ALT", "APPS", "ENTER", "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "SCROLL", "[", "INSERT", "HOME", "PG UP", "DELETE", "END", "PG DN", "U ARROW", "L ARROW", "D ARROW", "R ARROW", "NUM", "KP /", "KP *", "KP -", "KP +", "KP ENTER", "KP .", "KP 0", "KP 1", "KP 2", "KP 3", "KP 4", "KP 5", "KP 6", "KP 7", "KP 8", "KP 9", "]", ";", "'", ",", ".", "/"};
char ascii_codes[SCAN_CODE_NUM] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '`', '-', '=', 0, 0x08, 0, 0x09, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0A, 0x1B, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '[', 0, 0, 0, 0x7F, 0, 0, 0, 0, 0, 0, 0, '/', '*', '-', '+', 0x0A, '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ']', ';', '\'', ',', '.', '/'};
alt_u8 single_byte_make_code[SCAN_CODE_NUM] = {0x1C, 0x32, 0x21, 0x23, 0x24, 0x2B, 0x34, 0x33, 0x43, 0x3B, 0x42, 0x4B, 0x3A, 0x31, 0x44, 0x4D, 0x15, 0x2D, 0x1B, 0x2C, 0x3C, 0x2A, 0x1D, 0x22, 0x35, 0x1A, 0x45, 0x16, 0x1E, 0x26, 0x25, 0x2E, 0x36, 0x3D, 0x3E, 0x46, 0x0E, 0x4E, 0x55, 0x5D, 0x66, 0x29, 0x0D, 0x58, 0x12, 0x14, 0, 0x11, 0x59, 0, 0, 0, 0, 0x5A, 0x76, 0x05, 0x06, 0x04, 0x0C, 0x03, 0x0B, 0x83, 0x0A, 0x01, 0x09, 0x78, 0x07, 0x7E, 0x54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x77, 0, 0x7C, 0x7B, 0x79, 0, 0x71, 0x70, 0x69, 0x72, 0x7A, 0x6B, 0x73, 0x74, 0x6C, 0x75, 0x7D, 0x5B, 0x4C, 0x52, 0x41, 0x49, 0x4A};
alt_u8 multi_byte_make_code[SCAN_CODE_NUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x1F, 0, 0, 0x14, 0x27, 0x11, 0x2F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x70, 0x6C, 0x7D, 0x71, 0x69, 0x7A, 0x75, 0x6B, 0x72, 0x74, 0, 0x4A, 0, 0, 0, 0x5A, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

alt_up_ps2_dev *alt_up_ps2_open_dev(const char *name)
{

	if (strcmp(name, PS2_NAME) == 0)
	{
		alt_up_ps2_dev *device = malloc(sizeof(alt_up_ps2_dev));

		device->dev = 0;
		device->base = PS2_BASE;
		device->irq_id = PS2_IRQ;
		device->timeout = 0;
		device->device_type = PS2_KEYBOARD;
		device->pressedKeyAsciiValue = 0;
		return device;
	}

	return ((alt_up_ps2_dev *)0);
}

void alt_up_ps2_clear_fifo(alt_up_ps2_dev *ps2)
{
	ps2->pressedKeyAsciiValue = 0;
	return;
}

void initMockKeyboard(alt_up_ps2_dev *keyboard_device)
{
	BaseType_t taskStatus;

	taskStatus = xTaskCreate(mockKeyboardInput, "mockKeyboardInput", TASK_STACKSIZE, (void *)keyboard_device, MOCK_KEYBOARD_ISR_TASK_PRIORITY, NULL);
	handleTaskCreateError(taskStatus, "mockKeyboardInput");
}

// Poll inputs to simulate hardware interrupts.
void mockKeyboardInput(void *pvParameters)
{
	alt_up_ps2_dev *keyboard_device = (alt_up_ps2_dev *)pvParameters;

	while (1)
	{
		vTaskDelay(MOCK_KEYBOARD_POLL_DELAY / portTICK_PERIOD_MS);

		// Has a key been pressed?
		if (_kbhit() != 0)
		{
			//Since switches are simulated from function key input, we should check for function keys
			int keyboardInput = _getch(); //NOTE: this is Windows specific, not sure what the better way would be

			if (keyboardInput == 0 || keyboardInput == 224) //special keyboard input that generate two key stokes
			{
				wallSwitchHandler();
			}
			else if (keyboardInput == BUTTON_3_KEY || keyboardInput == BUTTON_2_KEY || keyboardInput == BUTTON_1_KEY || keyboardInput == BUTTON_0_KEY)
			{
				pushButtonHandler(keyboardInput);
			}
			else
			{
				// Put key in keyboard_device structure.
				keyboard_device->pressedKeyAsciiValue = keyboardInput;

				// trigger keyboard isr.
				if (keyboard_isr_handler)
				{
					(*keyboard_isr_handler)(keyboard_context, 0);
				}
				else
				{
					printf("New keyboard event. No keyboard_isr_handler defined\n");
				}

				if (keyboardInput == 'q' || keyboardInput == 'Q')
				{
					exit(0);
				}
			}
		}
	}
}

void wallSwitchHandler()
{
	//we have to check _getch() again.
	int keyboardInput = _getch();
	if (keyboardInput >= 59 && keyboardInput <= 68) //F1-F10
	{
		int switchIndex = keyboardInput - 59;
		//flip the bit associated with that key position
		switchValue = switchValue ^ (1U << switchIndex);
	}
}

void pushButtonHandler(int button)
{
	// Check that interrupts are enabled and there is not interrupt allready raised to the corresponding button.
	if (button == BUTTON_0_KEY && CHECK_BIT(buttonInterruptEnableMask, 0) && CHECK_BIT(buttonInterruptPendingMask, 0))
	{
		buttonInterruptPendingMask &= 0b1110;
		(*button_isr_handler)(button_context, 0);
		return;
	}
	else if (button == BUTTON_1_KEY && CHECK_BIT(buttonInterruptEnableMask, 1) && CHECK_BIT(buttonInterruptPendingMask, 1))
	{
		buttonInterruptPendingMask &= 0b1101;
		(*button_isr_handler)(button_context, 0);
		return;
	}
	else if (button == BUTTON_2_KEY && CHECK_BIT(buttonInterruptEnableMask, 2) && CHECK_BIT(buttonInterruptPendingMask, 2))
	{
		buttonInterruptPendingMask &= 0b1011;
		(*button_isr_handler)(button_context, 0);
		return;
	}
	else if (button == BUTTON_3_KEY && CHECK_BIT(buttonInterruptEnableMask, 3) && CHECK_BIT(buttonInterruptPendingMask, 3))
	{
		printf("RESTART SYSTEM\n"); //TODO: implament
	}
}

int decode_scancode(alt_up_ps2_dev *ps2, KB_CODE_TYPE *decode_mode, alt_u8 *buf, char *ascii)
{
	*decode_mode = KB_ASCII_MAKE_CODE;
	*buf = (alt_u8)ps2->pressedKeyAsciiValue;
	*ascii = (char)ps2->pressedKeyAsciiValue;
	return 0;
}
