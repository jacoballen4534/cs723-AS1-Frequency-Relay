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
        vTaskDelay(100 / portTICK_PERIOD_MS);
        FreqReading fr;
        insertIndex = 0;
        while(xQueueReceive(freqDisplayQ, (void *)&fr, (TickType_t) 0))
        {
            displayBuffer[insertIndex++] = fr;
        }  

        //output to UART
        int i;
        for(i = 0; i < insertIndex; i++)
        {
            printf("_fr,%.3lf,%.3lf,%u\n", displayBuffer[i].freq, displayBuffer[i].RoC,displayBuffer[i].timestamp);
        }
    }
}

//update UART output

//update LCD (keyboard input buffer?)
