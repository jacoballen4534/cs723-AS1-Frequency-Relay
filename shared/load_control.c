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
extern bool isManaging;
bool allConnected = false;
QueueHandle_t loadControlNotifyQ;
QueueHandle_t shedReconnectQ;
float firstShedLatency = 0.0;
float maxShedLatency = 0.0;
float minShedLatency = 0.0;

/////////////////////////////////////////
uint8_t shedVal[NUM_LOADS] = {0};
// TODO: Add mutex. Does this get its own mutex meaning updateLoadStatus will need to acquire two mutexes? or 1 mutex to cover loadStatus and xSwitchMutex
uint8_t loadStatus[NUM_LOADS] = {0}; //the final output of the device to the loads
//0 is OFF (shedded), 1 is ON ( not shedded)



//if we're reconnecting, and we turn on a load (unshedding) which has not yet been passed, it should not turn on, but should be evaluated when reached
//if we're reconnection, and we turn on a load that HAS been passed, it should not turn on until we reach the idle state


void vLoadControlTask(void *pvParameters);
void handleTaskCreateError(BaseType_t taskStatus, char *taskName);

void shedNextLoad(bool isShed)
{   
    uint8_t i = 0;
    if(isShed)
    {
        for (i = 0; i < NUM_LOADS; i++)
        {
            if(shedVal[i] == false)
            {
                shedVal[i] = true;
                //if we're tying to disable a load, but it is already manually disabled
                //also mark all subsequent manually switched off loads as shed to 'skip' over them
                while(switchVal[i] == false && i < NUM_LOADS-2)
                {
                    shedVal[i+1] = true;
                    i++;
                }
                break;
            }
        }

    }
    else
    {
        for (i = NUM_LOADS - 1; i >= 0; i--)
        {
            if(shedVal[i])
            {
                shedVal[i] = false;
                while(switchVal[i] == false && i >= 1)
                {
                    shedVal[i-1] = false;
                    i--;
                }
                break;
            }
        }
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

    //because we consider all switched off loads to be 'shedded' as well, we need to
    //find the subset of shedded loads which don't also have the switch turned on
    uint8_t i;
    uint8_t sheddedOnly[NUM_LOADS] = {0};
    for(i = 0; i < NUM_LOADS; i++)
    {
        sheddedOnly[i] = shedVal[i] & switchVal[i];
    }
    IOWR_ALTERA_AVALON_PIO_DATA(GREEN_LEDS_BASE, arrayToInt(sheddedOnly, NUM_LOADS));
}

void updateLoadStatus()
{
    allConnected = true;
    uint8_t i;

    for (i = 0; i < NUM_LOADS; i++)
    {
        loadStatus[i] = switchVal[i] & (shedVal[i] ^ (1U));
        if(shedVal[i] == true) allConnected = false; //if the load is shed even though the switch is up, we're not idle yet
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
            if(isManaging && switchVal[switchIndex] == true)
            {
                shedVal[switchIndex] = true;
            }
            updateLoadStatus();
            // if load was manually UNSHED and we are MANAGING. Set the load to shedVal = 1
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

                    if (firstShedLatency > maxShedLatency)
                    {
                        maxShedLatency = firstShedLatency;
                    }
                    if (firstShedLatency > minShedLatency || firstReading == true)
                    {
                        minShedLatency = firstShedLatency;
                        firstReading = false;
                    }
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
