#include <stdio.h>
#include "vars.h"
#include "wall_switch.h"
#include "taskMacros.h"
#include "load_shedder.h"
#include "freertos_includes.h"

// GLOBALS
bool allConnected = false;
QueueHandle_t loadControlNotifyQ;
QueueHandle_t shedReconnectQ;
bool newLatency = true;
float firstShedLatency = 0.0;
float maxShedLatency = 0.0;
float minShedLatency = 9999.0;
float avgShedLatency = 0.0;
uint32_t latencyCount = 0;

/////////////////////////////////////////
uint8_t shedVal[NUM_LOADS] = {0};
uint8_t latchedSwitches[NUM_LOADS] = {0};
// TODO: Add mutex. Does this get its own mutex meaning updateLoadStatus will need to acquire two mutexes? or 1 mutex to cover loadStatus and xSwitchMutex
uint8_t loadStatus[NUM_LOADS] = {0}; //the final output of the device to the loads
//0 is OFF (shedded), 1 is ON ( not shedded)

void vLoadControlTask(void *pvParameters);
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

void shedNextLoad(bool isShed)
{   
    int8_t i = 0;
    if(isShed == true)
    {
        i = 0;
        while(latchedSwitches[i] == 0 || shedVal[i] == 1)
        {
            i++;
            if (i >= NUM_LOADS) return;
        }
        shedVal[i] = 1;
    }
    else
    {
        i = NUM_LOADS - 1;
        while((shedVal[i] == 0) && (switchVal[i] == latchedSwitches[i]))
        {
            i--;
            if(i < 0) return;
        }
        shedVal[i] = 0;
        latchedSwitches[i] = switchVal[i];
    }
}

uint32_t arrayToInt(uint8_t* in, uint8_t len)
{
    uint32_t out = 0U;
    uint8_t i;
    for(i = 0; i < len; i++)
    {
        if(in[i] == true)
            out |= (1U << i);
    }
    return out;
}

void updateLEDs()
{
    //update red LEDs to load status
    IOWR_ALTERA_AVALON_PIO_DATA(RED_LEDS_BASE, arrayToInt(loadStatus, NUM_LOADS));
    IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LEDS_BASE, arrayToInt(shedVal, NUM_LOADS));
}

void updateLoadStatus()
{
    allConnected = true;
    uint8_t i;

    for (i = 0; i < NUM_LOADS; i++)
    {
        if((shedVal[i] == 1 && switchVal[i] == 1) || (latchedSwitches[i] != switchVal[i])) allConnected = false; //if the load is shed even though the switch is up, we're not done reconnecting
        loadStatus[i] = ((latchedSwitches[i] == 1) && (shedVal[i] == 0));
    }
    updateLEDs();
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
    bool firstReading = true;
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

        if (notifySource >= WALL_SWITCH_NOTIFICATION)
        {
            //read switch vals and update loads
            xSemaphoreTake(xSwitchMutex, portMAX_DELAY);
            uint8_t switchIndex = notifySource - WALL_SWITCH_NOTIFICATION;
            if(isManaging)
            {
                if(switchVal[switchIndex] == 0 && shedVal[switchIndex] == 1) shedVal[switchIndex] = 0; //manually turned off switches are not longer 'managed'
                latchedSwitches[switchIndex] = latchedSwitches[switchIndex] & switchVal[switchIndex];
            }
            else latchedSwitches[switchIndex] = switchVal[switchIndex];
            xSemaphoreGive(xSwitchMutex);
            updateLoadStatus();
            
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

                    if (firstShedLatency > maxShedLatency)
                    {
                        maxShedLatency = firstShedLatency;
                    }
                    if (firstShedLatency < minShedLatency || firstReading == true)
                    {
                        minShedLatency = firstShedLatency;
                        firstReading = false;
                    }
                    latencyCount = latencyCount + 1;
                    avgShedLatency = avgShedLatency * (float)(latencyCount-1)/(float)latencyCount + firstShedLatency/(float)latencyCount;
                    newLatency = true;
                }
            }

            xSemaphoreTake(xSwitchMutex, portMAX_DELAY);
            updateLoadStatus();
            xSemaphoreGive(xSwitchMutex);
        }
        else if (notifySource == WALL_SWITCH_RESET_NOTIFICATION)
        {
            uint8_t i;
            xSemaphoreTake(xSwitchMutex, portMAX_DELAY);
            for (i = 0; i < NUM_LOADS; i++)
            {
               latchedSwitches[i] = switchVal[i];
            }
            printf("UPDATING SWITCHES\n");
            updateLoadStatus();
            xSemaphoreGive(xSwitchMutex);
        }

        else if (notifySource == USER_INPUT_NOTIFICATION)
        {
            printf("new user input notification (button only)\n");
        }
    }
}   
