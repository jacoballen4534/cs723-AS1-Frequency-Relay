#include <stdio.h>
#include "vars.h"
#include "taskMacros.h"
#include "freertos_includes.h"

const uint32_t loadShedderNotificationValue = LOAD_SHEDDER_NOTIFICATION;
#define LOAD_SHEDDER_TASK_TIMEOUT 20
#define TIMER_START_STOP_WAIT_TIME 10

enum State
{
    IDLE = 0,
    SHED = 1,
    RECONNECT = 2
};

//feedback from loadControl so we know when to enter 'idle'
extern bool allConnected; //fixme: mutex guard

TimerHandle_t shedTimer;
SemaphoreHandle_t xThreshMutex;
double freqThresh;
double rocThresh;
SemaphoreHandle_t xIsMaintenanceMutex;
bool isMaintenance;
bool isManaging = false; //fixme: mutex guard

#define SHED_TIME_MS 500

//Condition 1: Freq below thresh
//Condition 2: RoC above

//When condition detected, move stable -> unstable, shed one load
//after 500 ms

void vLoadShedderTask(void *pvParameters);

bool timerOverflow = false; //FIXME: mutex guard

void timerShedISR(TimerHandle_t xTimer)
{
    timerOverflow = true;
}

int initLoadShedder(void)
{

    shedTimer = xTimerCreate("shedTimer", SHED_TIME_MS / portTICK_PERIOD_MS, pdTRUE, (void *)0, timerShedISR);

    // Start vDisplayOutputTask task
    BaseType_t taskStatus = xTaskCreate(vLoadShedderTask, "vLoadShedderTask", TASK_STACKSIZE, NULL, LOAD_SHEADDER_PRIORITY, NULL);
    handleTaskCreateError(taskStatus, "vLoadShedderTask");

    return 0;
}

void shedLoad(bool isShed, TickType_t timestamp)
{
    ShedRequest sr = {isShed, timestamp};
    xQueueSend(shedReconnectQ, (void *)&sr, portMAX_DELAY);
    BaseType_t result = xQueueSend(loadControlNotifyQ, (void *)&loadShedderNotificationValue, LOAD_SHEDDER_TASK_TIMEOUT);
    if (result == errQUEUE_FULL)
    {
        printf("load shed update fail as loadControllNotifyQ is full\n");
    }
}

void loadShedTick(FreqReading fr, enum State *state)
{
    //since we want to abort this function if in maintenance, make sure the mutex is released
    xSemaphoreTake(xIsMaintenanceMutex, USER_INPUT_BUFFER_BLOCK_TIME);
    bool localMaintenance = isMaintenance;
    xSemaphoreGive(xIsMaintenanceMutex);

    if(localMaintenance && (*state) == IDLE) //only enter maintenance mode from IDLE
    {
        return;
    }

    switch (*state)
    {
    case IDLE:
        if ((fr.freq < freqThresh) || (fr.RoC > rocThresh))
        {
            (*state) = SHED;
            isManaging = true;
            printf("_fsm,%d\n", (*state));
            shedLoad(true, fr.timestamp);
            xTimerStart(shedTimer, TIMER_START_STOP_WAIT_TIME);
        }
        break;
    case SHED:
        if (timerOverflow)
        {
            timerOverflow = false;
            shedLoad(true, 0);
        }
        if ((fr.freq > freqThresh) && (fr.RoC < rocThresh))
        {
            (*state) = RECONNECT;
            isManaging = true;
            printf("_fsm,%d\n", (*state));
            if (xTimerReset(shedTimer, TIMER_START_STOP_WAIT_TIME) != pdPASS)
            {
                printf("Could not restart timer in SHED state.\n");
            }
        }

        break;
    case RECONNECT:
        if (timerOverflow)
        {
            timerOverflow = false;
            shedLoad(false, 0);
        }

        if ((fr.freq < freqThresh) || (fr.RoC > rocThresh))
        {
            (*state) = SHED;
            isManaging = true;
            printf("_fsm,%d\n", (*state));
            if (xTimerReset(shedTimer, TIMER_START_STOP_WAIT_TIME) != pdPASS)
            {
                printf("Could not restart timer in RECONNECT state.\n");
            }
        }

        if (allConnected)
        {
            (*state) = IDLE;
            isManaging = false;
            printf("_fsm,%d\n", (*state));
            xTimerStop(shedTimer, TIMER_START_STOP_WAIT_TIME);
        }
        break;
    default:
        break;
    }
}

void vLoadShedderTask(void *pvParameters)
{
    enum State state = IDLE;
    printf("_fsm,%d\n", (state));
    while (1)
    {
        FreqReading fr;
        //TODO: Decide if this is portMAX_DELAY and assume that samples come in fast enough to handel timer or 0 delay and check if a new value was received.
        xQueueReceive(freqDataQ, (void *)&fr, portMAX_DELAY);
        xSemaphoreTake(xThreshMutex, portMAX_DELAY);
        loadShedTick(fr, &state);
        xSemaphoreGive(xThreshMutex);
    }
}
