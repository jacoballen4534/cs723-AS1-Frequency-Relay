#include <stdio.h>
#include "taskMacros.h"
#include "vars.h"

#ifdef __SIMULATION__
#include "mockIO.h"
#include "mockSystem.h"
// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#else
#include "io.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"
// Scheduler includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#endif

//contains task which polls switches periodically to determine values
//communicates status of all switches to load control task via shared memory (not necessary to mutex?)

// GLOBALS
uint8_t switchVal[NUM_LOADS] = {0};
SemaphoreHandle_t xSwitchMutex = NULL;
#define WALL_SWITCH_TASK_TIMEOUT 20
/////////////////////////////////////////

void vWallSwitchFrequencyTask(void *pvParameters);
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);
void intToArray(uint8_t * buf, uint32_t input, uint32_t array_length);

int initWallSwitches(void)
{
    // Start vWallSwitchFrequencyTask task
    BaseType_t taskStatus = xTaskCreate(vWallSwitchFrequencyTask, "vWallSwitchFrequencyTask", TASK_STACKSIZE, NULL, WALL_SWITCH_TASK_PRIORITY, NULL);
    handleTaskCreateError(taskStatus, "vWallSwitchFrequencyTask");

    xSwitchMutex = xSemaphoreCreateMutex();

    xSemaphoreTake(xSwitchMutex, portMAX_DELAY);
    intToArray(switchVal, IORD_ALTERA_AVALON_PIO_DATA(SLIDE_SWITCH_BASE), NUM_LOADS);
    xSemaphoreGive(xSwitchMutex);
    return 0;
}

void intToArray(uint8_t * buf, uint32_t input, uint32_t array_length)
{
    uint32_t mask = 0x0001;
    int i;
    for (i = 0; i < array_length; i++)
    {
        buf[i] = (input & mask);
        input = input >> 1;
    }
}

void vWallSwitchFrequencyTask(void *pvParameters)
{   
    const uint32_t notificationValue = WALL_SWITCH_NOTIFICATION;
    uint32_t previousRawSwitchValue = 0;
    while (1)
    {
        uint32_t rawSwitchValue = IORD_ALTERA_AVALON_PIO_DATA(SLIDE_SWITCH_BASE);

        if (rawSwitchValue != previousRawSwitchValue)
        {
            xSemaphoreTake(xSwitchMutex, portMAX_DELAY);
            intToArray(switchVal, rawSwitchValue, NUM_LOADS);
            xSemaphoreGive(xSwitchMutex);

            previousRawSwitchValue = rawSwitchValue;
        
        int i;
        for (i = 0; i < NUM_LOADS; i++)
        {
            printf("Switch %d: %u, ", i, switchVal[i]);
        }
        printf("\r\n");

            BaseType_t result = xQueueSend(loadControllNotifyQ, (void *)&notificationValue, WALL_SWITCH_TASK_TIMEOUT);
            if (result == errQUEUE_FULL)
            {
                printf("wall switch update fail as loadControllNotifyQ is full\n");
                continue; // Skip the vTaskDelay as it has allready waited WALL_SWITCH_TASK_TIMEOUT
            }
        }
        vTaskDelay(WALL_SWITCH_TASK_TIMEOUT);
    }
}