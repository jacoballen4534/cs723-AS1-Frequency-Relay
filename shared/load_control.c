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
/////////////////////////////////////////
int8_t shedCount = 0;
// TODO: Add mutex. Does this get its own mutex meaning updateLoadStatus will need to acquire two mutexes? or 1 mutex to cover loadStatus and xSwitchMutex
uint8_t loadStatus[NUM_LOADS] = {0}; //the final output of the device to the loads
//0 is ON (not shedded), 1 is OFF (shedded)

void vLoadControlTask(void *pvParameters);
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

void updateLoadStatus()
{
    uint8_t i;
    uint8_t shedsRemaining = shedCount;
    for (i = 0; i < NUM_LOADS; i++)
    {
        loadStatus[i] = switchVal[i];
        if(loadStatus[i] == 0 && shedsRemaining > 0)
        {
            loadStatus[i] = 1;
            shedsRemaining--;
        }
        // printf("Load %d: %d, ", i, loadStatus[i]);
    }
    // printf("\r\n");
}

uint8_t getSwitchedOffCount() //FIXME: inefficient to recount switches constantly, keep track of when detecting switch presses
{
    uint8_t cnt = 0;
    uint8_t i;
    for(i = 0; i < NUM_LOADS; i++)
        if(switchVal[i]) cnt++;

    return cnt;
}

int initLoadControl()
{
    shedCount = 0;
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
            bool isShed;
            uint8_t switchedOffCnt = getSwitchedOffCount();
            while(xQueueReceive(shedReconnectQ, (void *)&isShed, 0))
            {
                if(isShed) shedCount++;
                else shedCount--;
                
                if(shedCount <= 0) shedCount = 0;
                else if (shedCount > (NUM_LOADS - switchedOffCnt)) shedCount = (NUM_LOADS - switchedOffCnt);

                allConnected = (shedCount == 0) ? true : false; //FIXME: Mutex guard
            }

            xSemaphoreTake(xSwitchMutex, portMAX_DELAY);
            updateLoadStatus();
            xSemaphoreGive(xSwitchMutex);
        }

        else if (notifySource == USER_INPUT_NOTIFICATION)
        {
        }
    }
}   
