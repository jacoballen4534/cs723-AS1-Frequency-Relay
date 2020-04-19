#include <stdio.h>

#ifdef __SIMULATION__
#include "FreeRTOS.h"
#else
#include "freertos/FreeRTOS.h"
#endif
void shutDown(void);

// This function handles any errors when creating tasks.
void handleTaskCreateError(BaseType_t taskStatus, char *taskName)
{
	if (taskStatus == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
	{
		fprintf(stderr, "Could not allocate required memory for %s", taskName);
		shutDown();
	}
}
