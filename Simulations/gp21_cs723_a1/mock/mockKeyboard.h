#ifndef MOCK_KEYBOARD_H
#define MOCK_KEYBOARD_H

typedef unsigned long alt_dev;
typedef unsigned long alt_u32;
typedef unsigned char alt_u8;

#define MOCK_KEYBOARD_POLL_DELAY 10

typedef enum
{
	KB_ASCII_MAKE_CODE = 1,
	KB_BINARY_MAKE_CODE = 2,
	KB_LONG_BINARY_MAKE_CODE = 3,
	KB_BREAK_CODE = 4,
	KB_LONG_BREAK_CODE = 5,
	KB_INVALID_CODE = 6
} KB_CODE_TYPE;

typedef enum
{
	PS2_MOUSE = 0,
	PS2_KEYBOARD = 1,
	PS2_UNKNOWN = 2
} PS2_DEVICE;

typedef struct alt_up_ps2_dev
{
	alt_dev dev;
	unsigned int base;
	unsigned int irq_id;
	unsigned int timeout;
	PS2_DEVICE device_type;
	int pressedKeyAsciiValue;
} alt_up_ps2_dev;

alt_up_ps2_dev *alt_up_ps2_open_dev(const char *name);
void alt_up_ps2_clear_fifo(alt_up_ps2_dev *ps2);
void initMockKeyboard(alt_up_ps2_dev *keyboard_device);
void mockKeyboardInput(void *pvParameters);
void wallSwitchHandler();
void pushButtonHandler(int button);
int decode_scancode(alt_up_ps2_dev *ps2, KB_CODE_TYPE *decode_mode, alt_u8 *buf, char *ascii);

#endif /* MOCK_KEYBOARD_H */
