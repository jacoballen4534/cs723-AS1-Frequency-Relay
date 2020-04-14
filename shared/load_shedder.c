#include <stdio.h>
#include "vars.h"
#include "taskMacros.h"
#include "freertos_includes.h"

enum State {IDLE = 0, SHED = 1, RECONNECT = 2}; 
double freqThresh = 49.0;
double rocThresh = 8.0;

//extern bool isIdle;
bool isIdle = false;

alt_alarm shedTimer;
#define SHED_TIME_MS 500

//Condition 1: Freq below thresh
//Condition 2: RoC above 

//When condition detected, move stable -> unstable, shed one load
//after 500 ms 

void vLoadShedderTask(void *pvParameters);

bool timerOverflow = false; //FIXME: mutex guard
uint32_t timerShedISR()
{
    timerOverflow = true;
    return SHED_TIME_MS;
}


int initLoadShedder(void)
{
    // Start vDisplayOutputTask task
    BaseType_t taskStatus = xTaskCreate(vLoadShedderTask, "vLoadShedderTask", TASK_STACKSIZE, NULL, DISPLAY_TASK_PRIORITY, NULL);
    handleTaskCreateError(taskStatus, "vLoadShedderTask");

    return 0;
}

void shedLoad(bool isShed)
{
    xQueueSend(shedReconnectQ, (void *)&isShed, portMAX_DELAY);
}

void loadShedTick(FreqReading fr, enum State* state)
{
    switch(*state)
    {
        case IDLE:
            if((fr.freq < freqThresh) || (fr.RoC > rocThresh))
            {
                (*state) = SHED;
                shedLoad(true);
                alt_alarm_start(&shedTimer, SHED_TIME_MS, timerShedISR, NULL);
            }
        break;
        case SHED:
            if(timerOverflow)
            {
                timerOverflow = false;
                shedLoad(true);
            }
            if( (fr.freq > freqThresh) && (fr.RoC < rocThresh))
            {
                (*state) = RECONNECT;
                alt_alarm_stop(&shedTimer);
                alt_alarm_start(&shedTimer, SHED_TIME_MS, timerShedISR, NULL);
            }

        break;
        case RECONNECT:
            if(timerOverflow)
            {
                timerOverflow = false;
                shedLoad(false);
            }
            if(isIdle)
            {
                (*state) = IDLE;
                alt_alarm_stop(&shedTimer);
            }
        break;
        default:
        break;
    }
}

void vLoadShedderTask(void *pvParameters)
{
    enum State state = IDLE;
    while(1)
    {
        FreqReading fr;
        xQueueReceive(freqDataQ, (void *)&fr, (TickType_t) 0);
        loadShedTick(fr, &state);
    }
}