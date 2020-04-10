#include <stdio.h>
#include "vars.h"
#include "taskMacros.h"

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

//This combines the load shedder output and the wall switch output
//in a "logical AND" fashion, and updates the relevant output LEDs
//Controls load status LEDs, as well as load shedder LEDs
//Can disable input from load shedder based on maintenace mode

// GLOBALS
extern uint8_t switchVal[NUM_LOADS];
extern SemaphoreHandle_t xSwitchMutex;
/////////////////////////////////////////
uint8_t shedderStatus[NUM_LOADS] = {0};
uint8_t loadStatus[NUM_LOADS] = {0}; //the final output of the device to the loads

void vLoadControlTask(void *pvParameters);
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

void updateLoadStatus()
{
    int i;
    for(i = 0; i < NUM_LOADS; i++)
    {
        loadStatus[i] = shedderStatus[i] & switchVal[i];
        printf("Load %d: %d, ", i, loadStatus[i]);
    }
    printf("\r\n");
}

int initLoadControl()
{
    int i;
    for(i = 0; i < NUM_LOADS; i++)
    {
        shedderStatus[i] = 1;
    }
    updateLoadStatus();

    BaseType_t taskStatus = xTaskCreate(vLoadControlTask, "vLoadControlTask", TASK_STACKSIZE, NULL, LOAD_CONTROL_TASK_PRIORITY, NULL);
    handleTaskCreateError(taskStatus, "vLoadControlTask");

    return 0;
}

void vLoadControlTask(void *pvParameters)
{
    uint32_t notifySource = 0;
    while (1)
    {
        //will awaken when signaled by UserInputTask, LoadShedderTask or wallSwitchTask change
        BaseType_t queueReceiveStatus = xQueueReceive(loadControllNotifyQ, (void *)&notifySource, portMAX_DELAY);
        if (queueReceiveStatus == pdFALSE)
        {
            // Something went wrong
            continue;
        }

        if (notifySource == WALL_SWITCH_NOTIFICATION)
        {
            //read switch vals and update loads
            xSemaphoreTake(xSwitchMutex, portMAX_DELAY);
            updateLoadStatus();
            xSemaphoreGive(xSwitchMutex);
        }
        else if (notifySource == USER_INPUT_NOTIFICATION)
        {
        }
        else if (notifySource == LOAD_SHEDDER_NOTIFICATION)
        {
        }
    }
}   