#include <stdio.h>
#include "vars.h"
#include "wall_switch.h"
#include "user_input.h"
#include "load_shedder.h"
#include "load_control.h"

#include "taskMacros.h"
#include "freertos_includes.h"

// Forward declare
void shutDown(void);
/////////////////////////

#define PRINT_MUTEX_BLOCK_TIME 5

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
    uint8_t mailBoxReceive;
    char LCDPrintBuffer[USER_INPUT_BUFFER_LENGTH + 10] = {0}; // Allow room for Roc or Fre and terminator
    printf("_start\n");
    while (1)
    {
        vTaskDelay(20 / portTICK_PERIOD_MS);
        // ---------------------- Send frequency, roc and timestamps ------------------------
        FreqReading fr;
        insertIndex = 0;
        while (xQueueReceive(freqDisplayQ, (void *)&fr, (TickType_t)0))
        {
            displayBuffer[insertIndex++] = fr;
        }

        for (i = 0; i < insertIndex; i++)
        {
            printf("_f,%.3lf,%.3lf,%u\n", displayBuffer[i].freq, displayBuffer[i].RoC, displayBuffer[i].timestamp);
        }

        // ---------------------- Send load status ----------------------
        if (xQueueReceive(newLoadStatusToDisplayQ, (void *)&mailBoxReceive, (TickType_t)0))
        {
            printf("_l,");
            for (i = 0; i < NUM_LOADS; i++)
            {
                printf("%d,", loadStatus[i]);
            }
            printf("\r\n");
        }

        // ---------------------- Send switch status ----------------------
        if (xQueueReceive(newSwitchValToDisplayQ, (void *)&mailBoxReceive, (TickType_t)0))
        {
            printf("_s,");
            for (i = 0; i < NUM_LOADS; i++)
            {
                printf("%u,", switchVal[i]);
            }
            printf("\r\n");
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);

        // ---------------------- Send thresholds ----------------------
        if (xQueueReceive(newThresholdToDisplayQ, (void *)&mailBoxReceive, (TickType_t)0))
        {
            xSemaphoreTake(xThreshMutex, PRINT_MUTEX_BLOCK_TIME);
            printf("_fth,%.3lf\n", freqThresh);
            printf("_r,%.3lf\n", rocThresh);
            xSemaphoreGive(xThreshMutex);
        }

        // ---------------------- Send latency ----------------------
        if (xQueueReceive(newLatencyToDisplayQ, (void *)&mailBoxReceive, (TickType_t)0))
        {
            printf("_lt,%.0f,%.0f,%.0f,%.2f\n", firstShedLatency, minShedLatency, maxShedLatency, avgShedLatency); //FIXME: Mutex guard
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);

        // ---------------------- Send LCD ----------------------
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
                printf("_lcd,%s\n", LCDPrintBuffer);
                newUserInputValue = false;
                xSemaphoreGive(xUserInputBufferMutex);
            }
        }

        // ---------------------- Send maintenance status ----------------------
        //fixme: mutex guard
        if (xQueueReceive(newIsMaintenanceToDisplayQ, (void *)&mailBoxReceive, (TickType_t)0))
        {
            bool localIsMaintenance = isMaintenance; //don't lock up the mutex in a lengthy print
            printf("_m,%u\n", localIsMaintenance);
        }
    }
}
