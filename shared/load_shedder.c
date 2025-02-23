#include <stdio.h>
#include "vars.h"
#include "load_control.h"
#include "taskMacros.h"
#include "freertos_includes.h"

const uint32_t loadShedderNotificationValue = LOAD_SHEDDER_NOTIFICATION;
const uint32_t forceWallSwitchNotificationValue = WALL_SWITCH_RESET_NOTIFICATION;

#define LOAD_SHEDDER_TASK_TIMEOUT 20
#define TIMER_START_STOP_WAIT_TIME 10

enum State
{
    IDLE = 0,
    SHED = 1,
    RECONNECT = 2
};

TimerHandle_t shedTimer;
SemaphoreHandle_t xThreshMutex;
QueueHandle_t timerQ;
double freqThresh;
double rocThresh;
SemaphoreHandle_t xIsMaintenanceMutex;
bool isMaintenance = false;
bool isManaging = false;
SemaphoreHandle_t xIsManagingMutex;

#define SHED_TIME_MS 500

void vLoadShedderTask(void *pvParameters);

void timer_shed_isr(TimerHandle_t xTimer)
{
    uint8_t overflowVal = 1;
    BaseType_t queueSendStatus = xQueueSendFromISR(timerQ, (void *)&overflowVal, NULL);
    if (queueSendStatus != pdTRUE)
    {
        printf("timerQ is full (timer callback)\n");
    }
}

int initLoadShedder(void)
{
    shedTimer = xTimerCreate("shedTimer", SHED_TIME_MS / portTICK_PERIOD_MS, pdTRUE, (void *)0, timer_shed_isr);

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

void forceUpdateSwitches()
{
    BaseType_t result = xQueueSend(loadControlNotifyQ, (void *)&forceWallSwitchNotificationValue, LOAD_SHEDDER_TASK_TIMEOUT);
    if (result == errQUEUE_FULL)
    {
        printf("wall switch force update fail as loadControllNotifyQ is full\n");
    }
}

void loadShedTick(FreqReading fr, enum State *state)
{
    //since we want to abort this function if in maintenance, make sure the mutex is released
    xSemaphoreTake(xIsMaintenanceMutex, USER_INPUT_BUFFER_BLOCK_TIME);
    bool localMaintenance = isMaintenance;
    xSemaphoreGive(xIsMaintenanceMutex);

    uint8_t flushTimerFlagInto;

    if (localMaintenance && (*state) == IDLE) //only enter maintenance mode from IDLE
    {
        return;
    }

    switch (*state)
    {
    case IDLE:
        if ((fr.freq < freqThresh) || (fr.RoC > rocThresh))
        {
            (*state) = SHED;
            xSemaphoreTake(xIsManagingMutex, portMAX_DELAY);
            isManaging = true;
            xSemaphoreGive(xIsManagingMutex);
            printf("_fsm,%d\n", (*state));
            shedLoad(true, fr.timestamp);
            xTimerStart(shedTimer, TIMER_START_STOP_WAIT_TIME);
        }
        break;
    case SHED:
        while (xQueueReceive(timerQ, &flushTimerFlagInto, 0))
        {
            shedLoad(true, 0);
        }
        if ((fr.freq > freqThresh) && (fr.RoC < rocThresh))
        {
            (*state) = RECONNECT;
            xSemaphoreTake(xIsManagingMutex, portMAX_DELAY);
            isManaging = true;
            xSemaphoreGive(xIsManagingMutex);
            printf("_fsm,%d\n", (*state));
            if (xTimerReset(shedTimer, TIMER_START_STOP_WAIT_TIME) != pdPASS)
            {
                printf("Could not restart timer in SHED state.\n");
            }
        }

        break;
    case RECONNECT:
        while (xQueueReceive(timerQ, &flushTimerFlagInto, 0))
        {
            shedLoad(false, 0);
        }

        if ((fr.freq < freqThresh) || (fr.RoC > rocThresh))
        {
            (*state) = SHED;
            xSemaphoreTake(xIsManagingMutex, portMAX_DELAY);
            isManaging = true;
            xSemaphoreGive(xIsManagingMutex);
            printf("_fsm,%d\n", (*state));
            if (xTimerReset(shedTimer, TIMER_START_STOP_WAIT_TIME) != pdPASS)
            {
                printf("Could not restart timer in RECONNECT state.\n");
            }
        }

        xSemaphoreTake(xAllConnectedMutex, portMAX_DELAY);
        bool localAllConnected = allConnected;
        xSemaphoreGive(xAllConnectedMutex);

        if (localAllConnected)
        {
            (*state) = IDLE;
            xSemaphoreTake(xIsManagingMutex, portMAX_DELAY);
            isManaging = false;
            xSemaphoreGive(xIsManagingMutex);
            forceUpdateSwitches();
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
