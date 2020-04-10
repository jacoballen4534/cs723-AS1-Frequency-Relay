#include <stdio.h>
#include "vars.h"
#include "taskMacros.h"
#include "freertos_includes.h"

void vDisplayOutputTask(void *pvParameters);
void initialiseBuffer();

FreqReading displayBuffer[DISPLAY_BUFFER_LENGTH];
int insertIndex = DISPLAY_BUFFER_LENGTH - 1;
;

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
    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        FreqReading fr;
        while(xQueueReceive(freqDisplayQ, (void *)&fr, (TickType_t) 0))
        {
            insertIndex = (insertIndex + 1) % DISPLAY_BUFFER_LENGTH;
            displayBuffer[insertIndex] = fr;
        }

        //output to UART
        printf("SNAPSHOT:\r\n");
        int i;
        for(i = 0; i < DISPLAY_BUFFER_LENGTH; i++)
        {
            int j = (insertIndex - i);
            if (j < 0)
                j = DISPLAY_BUFFER_LENGTH + j;

            printf("Reading,%lf,", displayBuffer[j].freq);
            printf("%lf,", displayBuffer[j].RoC);
            printf("%u\r\n", displayBuffer[j].timestamp);
        }
        printf("\r\n");
    }
}

//update UART output

//update LCD (keyboard input buffer?)
