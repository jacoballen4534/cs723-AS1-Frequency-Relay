#include <stdio.h>
#include "vars.h"
#include "taskMacros.h"
#include "freertos_includes.h"

const uint32_t notificationValue = LOAD_SHEDDER_NOTIFICATION;
#define LOAD_SHEDDER_TASK_TIMEOUT 20

enum State {IDLE = 0, SHED = 1, RECONNECT = 2}; 
double freqThresh = 49.0;
double rocThresh = 8.0;

//feedback from loadControl so we know when to enter 'idle'
extern bool allConnected; //fixme: mutex guard

alt_alarm shedTimer;
#define SHED_TIME_MS 500

//Condition 1: Freq below thresh
//Condition 2: RoC above 

//When condition detected, move stable -> unstable, shed one load
//after 500 ms 

void vLoadShedderTask(void *pvParameters);

bool timerOverflow = false; //FIXME: mutex guard
alt_u32 timerShedISR(void *context)
{
    timerOverflow = true;
    return SHED_TIME_MS;
}


int initLoadShedder(void)
{
#ifdef __SIMULATION__
    initMockTimer(&shedTimer); // If there are multiple timers, each one will need to be initialised individually.
#endif

    // Start vDisplayOutputTask task
    BaseType_t taskStatus = xTaskCreate(vLoadShedderTask, "vLoadShedderTask", TASK_STACKSIZE, NULL, LOAD_SHEADDER_PRIORITY, NULL);
    handleTaskCreateError(taskStatus, "vLoadShedderTask");

    return 0;
}

void shedLoad(bool isShed)
{
    xQueueSend(shedReconnectQ, (void *)&isShed, portMAX_DELAY);
    BaseType_t result = xQueueSend(loadControlNotifyQ, (void *)&notificationValue, LOAD_SHEDDER_TASK_TIMEOUT);
    if (result == errQUEUE_FULL)
    {
        printf("load shed update fail as loadControllNotifyQ is full\n");
    }
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

            if((fr.freq < freqThresh) || (fr.RoC > rocThresh))
            {
                (*state) = SHED;
                alt_alarm_stop(&shedTimer);
                alt_alarm_start(&shedTimer, SHED_TIME_MS, timerShedISR, NULL);
            }

            if(allConnected)
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
        //TODO: Decide if this is portMAX_DELAY and assume that samples come in fast enough to handel timer or 0 delay and check if a new value was received.
        xQueueReceive(freqDataQ, (void *)&fr, portMAX_DELAY);
        loadShedTick(fr, &state);
    }
}