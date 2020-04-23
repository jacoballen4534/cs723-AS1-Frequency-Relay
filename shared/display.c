#include <stdio.h>
#include "vars.h"
#include "taskMacros.h"
#include "freertos_includes.h"

// Forward declare
void shutDown(void);

#define PRINT_MUTEX_BLOCK_TIME 5

extern uint8_t switchVal[NUM_LOADS];
extern uint8_t loadStatus[NUM_LOADS];

// LCD defines
#define ESC 27
#define CLEAR_LCD_STRING "[2J"
FILE *LCD = NULL;

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

    LCD = fopen(CHARACTER_LCD_NAME, "w");

    if (LCD == NULL)
    {
        printf("Could not open LCD\n");
        shutDown();
    }


    return 0;
}

void initialiseBuffer()
{
    int i;
    for (i = 0; i < DISPLAY_BUFFER_LENGTH; i++)
    {
        displayBuffer[i].freq = 0;
        displayBuffer[i].RoC = 0;
        displayBuffer[i].timestamp = (TickType_t)0;
    }
}

void vDisplayOutputTask(void *pvParameters)
{
    int i;
    char LCDPrintBuffer[USER_INPUT_BUFFER_LENGTH + 10] = {0}; // Allow room for Roc or Fre and terminator
    while (1)
    {
        vTaskDelay(20 / portTICK_PERIOD_MS);
        FreqReading fr;
        insertIndex = 0;
        while (xQueueReceive(freqDisplayQ, (void *)&fr, (TickType_t)0))
        {
            displayBuffer[insertIndex++] = fr;
        }

        //TODO: Use mutex
        printf("_l,");
        for (i = 0; i < NUM_LOADS; i++)
        {
            printf("%d,", loadStatus[i]);
        }
        printf("\r\n");

        //TODO: Use mutex
        printf("_s,");
        for (i = 0; i < NUM_LOADS; i++)
        {
            printf("%u,", switchVal[i]);
        }
        printf("\r\n");

        for (i = 0; i < insertIndex; i++)
        {
            printf("_f,%.3lf,%.3lf,%u\n", displayBuffer[i].freq, displayBuffer[i].RoC, displayBuffer[i].timestamp);
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);

        xSemaphoreTake(xThreshMutex, PRINT_MUTEX_BLOCK_TIME);
        printf("_fth,%.3lf\n", freqThresh);
        printf("_r,%.3lf\n", rocThresh);
        xSemaphoreGive(xThreshMutex);

        printf("_lt,%.0f,%.0f,%.0f\n", firstShedLatency, minShedLatency, maxShedLatency); //FIXME: Mutex guard

        vTaskDelay(20 / portTICK_PERIOD_MS);
        //update LCD (keyboard input buffer?)
        if (newUserInputValue)
        {
            if (xSemaphoreTake(xUserInputBufferMutex, PRINT_MUTEX_BLOCK_TIME))
            {
                // Write to display.
                if (updateType == Frequency)
                {
                    sprintf(LCDPrintBuffer, "Fre: %s", userInputBuffer);
                }
                else
                {
                    sprintf(LCDPrintBuffer, "Roc: %s", userInputBuffer);
                }

                fprintf(LCD, "%c%s", ESC, CLEAR_LCD_STRING);
                fprintf(LCD, "%s\n", LCDPrintBuffer);

                newUserInputValue = false;
                xSemaphoreGive(xUserInputBufferMutex);
            }
        }
    }
}
