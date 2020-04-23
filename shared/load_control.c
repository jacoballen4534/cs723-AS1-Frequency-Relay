#include <stdio.h>
#include "vars.h"
#include "taskMacros.h"
#include "freertos_includes.h"

//This combines the load shedder output and the wall switch output
//in a "logical AND" fashion, and updates the relevant output LEDs
//Controls load status LEDs, as well as load shedder LEDs
//Can disable input from load shedder based on maintenace mode

// GLOBALS
extern uint8_t switchVal[NUM_LOADS];
extern SemaphoreHandle_t xSwitchMutex;
bool allConnected = false;
QueueHandle_t loadControlNotifyQ;
QueueHandle_t shedReconnectQ;
float firstShedLatency = 0.0;


/////////////////////////////////////////
uint8_t shedVal[NUM_LOADS] = {0};
// TODO: Add mutex. Does this get its own mutex meaning updateLoadStatus will need to acquire two mutexes? or 1 mutex to cover loadStatus and xSwitchMutex
uint8_t loadStatus[NUM_LOADS] = {0}; //the final output of the device to the loads
//0 is ON (not shedded), 1 is OFF (shedded)

void vLoadControlTask(void *pvParameters);
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

void shedNextLoad(bool isShed)
{   
    uint8_t i = 0;
    if(isShed)
    {
        for (i = 0; i < NUM_LOADS; i++)
        {
            printf("Evaluating number %d\n", i);
            if(shedVal[i] == false && switchVal[i] == false)
            {
                printf("Shedding %d\n", i);
                shedVal[i] = true;
                break;
            }
        }

    }
    else
    {
        for (i = NUM_LOADS - 1; i >= 0; i--)
        {
            if(shedVal[i] && switchVal[i] == false)
            {
                shedVal[i] = false;
                break;
            }
        }
    }

    
}

void updateLoadStatus()
{
    allConnected = true;
    uint8_t i;
    for (i = 0; i < NUM_LOADS; i++)
    {
        if(shedVal[i]) allConnected = false;
        loadStatus[i] = switchVal[i] | shedVal[i];
    }
}

int initLoadControl()
{
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
        BaseType_t queueReceiveStatus = xQueueReceive(loadControlNotifyQ, (void *)&notifySource, portMAX_DELAY);
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
        else if (notifySource == LOAD_SHEDDER_NOTIFICATION)
        {
            //receive queue values, then update loads
            ShedRequest shedRequest;
            while(xQueueReceive(shedReconnectQ, (void *)&shedRequest, 0))
            {
                shedNextLoad(shedRequest.isShed);
                
                if (shedRequest.timestamp != 0) // Ignore requests that are not the initial shed
                {
	                firstShedLatency = (float)(xTaskGetTickCount() - shedRequest.timestamp) / (float)portTICK_PERIOD_MS;
                }
            }

            xSemaphoreTake(xSwitchMutex, portMAX_DELAY);
            updateLoadStatus();
            xSemaphoreGive(xSwitchMutex);
        }

        else if (notifySource == USER_INPUT_NOTIFICATION)
        {
            printf("new user input notification (button only)\n");
        }
    }
}   
