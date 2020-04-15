#ifndef MOCK_TIMER_H
#define MOCK_TIMER_H
#include "FreeRTOS.h"
#include "timers.h" //Freertos timer

#define EINVAL 22        /* Invalid argument */
#define FREERTOSERROR 20 /* Custom error value */
#define NUMBER_OF_TIMERS 1
#define MOCK_TIMER_WAIT_TIME 10

typedef unsigned long alt_u32;

struct alt_alarm_s
{
    alt_u32 (*callback)(void *context); /* callback function. The return 
                          * value is the period for the next callback; where 
                          * zero indicates that the alarm should be removed 
                          * from the list. 
                          */

    void *context; /* Argument for the callback */
    TimerHandle_t freertosTimer;
    int freertosTimerID;
};

typedef struct alt_alarm_s alt_alarm;

int initMockTimer(alt_alarm *alarm);
void timerCallback(TimerHandle_t xTimer);

int alt_alarm_start(alt_alarm *the_alarm, alt_u32 nticks, alt_u32 (*callback)(void *context), void *context);
void alt_alarm_stop(alt_alarm *the_alarm);

alt_alarm *timers[NUMBER_OF_TIMERS];

#endif /* MOCK_TIMER_H */