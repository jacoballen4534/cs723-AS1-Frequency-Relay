#ifndef USER_INPUT_H
#define USER_INPUT_H
#include "vars.h"

extern char userInputBuffer[USER_INPUT_BUFFER_LENGTH + 1];  // Allow a /0 to be put on the end
extern bool newUserInputValue;								// Only update the LCD on new values to prevent flickering.
extern UpdateType updateType;								// Indicate what value is being updated
extern SemaphoreHandle_t xUserInputBufferMutex;

#endif
