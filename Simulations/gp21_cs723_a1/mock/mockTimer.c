#include <stdio.h>
#include "mockTimer.h"
#include "projdefs.h"

static int timerID = 0;

int initMockTimer(alt_alarm *alarm)
{
    alarm->freertosTimerID = timerID++;
    if (alarm->freertosTimerID >= NUMBER_OF_TIMERS)
    {
        printf("There was an error initializing a mockTimer with id %d \n", alarm->freertosTimerID);
        return 1;
    }

    timers[alarm->freertosTimerID] = alarm;
    alarm->freertosTimer = xTimerCreate("", 1, pdFALSE, (void *)alarm->freertosTimerID, timerCallback);

    return 0;
}

void timerCallback(TimerHandle_t xTimer)
{
    int timerID = pvTimerGetTimerID(xTimer);
    printf("Freertos timer callback triggered by timer  %d.\n", timerID);

    if (timerID >= NUMBER_OF_TIMERS)
    {
        printf("Invalid mock timer isr with id %d \n", timerID);
        return;
    }

    alt_alarm *alarm = timers[timerID];

    if (!alarm || !alarm->callback)
    {
        printf("There was a timer ISR but not alarm handler.\n");
        return;
    }

    alt_u32 nextTimerPeriod = alarm->callback(alarm->context);

    if (nextTimerPeriod == 0)
    {
        // Stop the timer
        xTimerStop(xTimer, MOCK_TIMER_WAIT_TIME);
        return;
    }

    xTimerChangePeriod(xTimer, nextTimerPeriod / portTICK_PERIOD_MS, MOCK_TIMER_WAIT_TIME);
}

void alt_alarm_stop(alt_alarm *alarm)
{
    printf("alt_alarm_stop.\n");
    if (!alarm || !alarm->freertosTimer)
    {
        printf("There was an error stopping a timer\n");
        return;
    }
    // Stop the timer
    printf("Stopping alarm with id %d\n", alarm->freertosTimerID);

    xTimerStop(alarm->freertosTimer, MOCK_TIMER_WAIT_TIME);
}

int alt_alarm_start(alt_alarm *alarm, alt_u32 nticks, alt_u32 (*callback)(void *context), void *context)
{
    if (!alarm || !nticks || !callback)
    {
        return -EINVAL;
    }

    if (!alarm->freertosTimer)
    {
        // Timer was not initalized
        printf("freertosTimer was not initalized");
        return FREERTOSERROR;
    }

    alarm->callback = callback;
    alarm->context = context;

    // Update the period and also starts the timer.
    if (xTimerChangePeriod(alarm->freertosTimer, nticks / portTICK_PERIOD_MS, MOCK_TIMER_WAIT_TIME) != pdPASS)
    {
        printf("There was an error when updating freertos timer period");
        return FREERTOSERROR;
    }

    return 0;
}
