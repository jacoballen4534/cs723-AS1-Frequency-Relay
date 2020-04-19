#include <stdio.h>
#include "vars.h"
#include "taskMacros.h"
#include "freertos_includes.h"

#define PRINT_MUTEX_BLOCK_TIME 5

extern uint8_t switchVal[NUM_LOADS];
extern uint8_t loadStatus[NUM_LOADS];

void vDisplayOutputTask(void *pvParameters);
void initialiseBuffer();

FreqReading displayBuffer[DISPLAY_BUFFER_LENGTH];
int insertIndex = DISPLAY_BUFFER_LENGTH - 1;

int initDisplay(void)
{
    // Start vDisplayOutputTask task
    BaseType_t taskStatus = xTaskCreate(vDisplayOutputTask, "vDisplayOutputTask", TASK_STACKSIZE, NULL, DISPLAY_TASK_PRIORITY, NULL);
    handleTaskCreateError(taskStatus, "vDisplayOutputTask");
    initialiseBuffer();
    return 0;
}

void initialiseBuffer()
{
    int i;
    for(i = 0; i < DISPLAY_BUFFER_LENGTH; i++)
    {
        displayBuffer[i].freq = 0;
        displayBuffer[i].RoC = 0;
        displayBuffer[i].timestamp = (TickType_t) 0;
    }
}


void vDisplayOutputTask(void *pvParameters)
{
    int i;
    while (1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        FreqReading fr;
        insertIndex = 0;
        while (xQueueReceive(freqDisplayQ, (void *)&fr, (TickType_t)0))
        {
            displayBuffer[insertIndex++] = fr;
        }  

            //TODO: Use mutex
            printf("_lo,");
            for (i = 0; i < NUM_LOADS; i++)
            {
                printf("%d,", loadStatus[i]);
            }
            printf("\r\n");

            //TODO: Use mutex
            printf("_sw,");
            for (i = 0; i < NUM_LOADS; i++)
            {
                printf("%u,", switchVal[i]);
            }
            printf("\r\n");

        for (i = 0; i < insertIndex; i++)
        {
            printf("_fr,%.3lf,%.3lf,%u\n", displayBuffer[i].freq, displayBuffer[i].RoC, displayBuffer[i].timestamp);
        }

        xSemaphoreTake(xThreshMutex, PRINT_MUTEX_BLOCK_TIME);
        printf("_fth,%.3lf\n", freqThresh);
        printf("_rth,%.3lf\n", rocThresh);
        xSemaphoreGive(xThreshMutex);

        //update LCD (keyboard input buffer?)
        if (newUserInputValue)
        {
            if (xSemaphoreTake(xUserInputBufferMutex, PRINT_MUTEX_BLOCK_TIME))
            {
                // Write to display.
                updateType == Frequency ? printf("Frequency: ") : printf("Roc: ");
                for (i = 0; i < userInputBufferIndex; i++)
                {
                    printf("%c", userInputBuffer[i]);
                }
                printf("\n");
                newUserInputValue = false;
                xSemaphoreGive(xUserInputBufferMutex);
            }
        }
    }
}
