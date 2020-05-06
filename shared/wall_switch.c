#include <stdio.h>
#include "taskMacros.h"
#include "vars.h"
#include "freertos_includes.h"

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

//contains task which polls switches periodically to determine values
//communicates status of all switches to load control task via shared memory (not necessary to mutex?)

// GLOBALS
uint8_t switchVal[NUM_LOADS] = {0};
SemaphoreHandle_t xSwitchMutex = NULL;
#define WALL_SWITCH_TASK_TIMEOUT 20
uint32_t wallSwitchNotificationValue = WALL_SWITCH_NOTIFICATION;

/////////////////////////////////////////

void vWallSwitchFrequencyTask(void *pvParameters);
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);
void intToArray(uint8_t *buf, uint32_t input, uint32_t array_length);

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

void intToArray(uint8_t *buf, uint32_t input, uint32_t array_length)
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
    uint32_t previousRawSwitchValue = 0;
    while (1)
    {
        uint32_t rawSwitchValue = IORD_ALTERA_AVALON_PIO_DATA(SLIDE_SWITCH_BASE);

        if (rawSwitchValue != previousRawSwitchValue) //There has been a change in switches
        {

            uint32_t diff = previousRawSwitchValue ^ rawSwitchValue; // Find the difference in switches

            xSemaphoreTake(xSwitchMutex, portMAX_DELAY);
            uint8_t i;
            for (i = 0; i < NUM_LOADS; i++)
            {
                if (CHECK_BIT(diff, i)) // Only accept the first switch change (Can only send 1 change at a time through queue)
                {
                    // Update switchVal to reflect this single change
                    if (CHECK_BIT(rawSwitchValue, i))
                        switchVal[i] = 1;
                    else
                        switchVal[i] = 0;

                    wallSwitchNotificationValue = WALL_SWITCH_NOTIFICATION + i;
                    break;
                }
            }
            xSemaphoreGive(xSwitchMutex);

            // Update previousRawSwitchValue with the change that has been accepted.
            // If multiple switches have been changed, the next check will still show a diff.
            if (CHECK_BIT(rawSwitchValue, i))
                previousRawSwitchValue |= (1 << i);
            else
                previousRawSwitchValue &= ~(1 << i);

            // Send the change
            BaseType_t result = xQueueSend(loadControlNotifyQ, (void *)&wallSwitchNotificationValue, WALL_SWITCH_TASK_TIMEOUT);
            if (result == errQUEUE_FULL)
            {
                printf("wall switch update fail as loadControllNotifyQ is full\n");
                continue; // Skip the vTaskDelay as it has allready waited WALL_SWITCH_TASK_TIMEOUT
            }
        }
        vTaskDelay(WALL_SWITCH_TASK_TIMEOUT / portTICK_PERIOD_MS);
    }
}
