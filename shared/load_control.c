#include <system.h>
#include <altera_avalon_pio_regs.h>
#include "taskMacros.h"
#include <stdio.h>
#include "vars.h"

#ifdef __SIMULATION__
#include "mockIO.h"
#include "mockSystem.h"
// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#else
#include "io.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"
// Scheduler includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#endif


//This combines the load shedder output and the wall switch output
//in a "logical AND" fashion, and updates the relevant output LEDs
//Controls load status LEDs, as well as load shedder LEDs
//Can disable input from load shedder based on maintenace mode

// GLOBALS
extern uint8_t switchVal[NUM_SWITCHES];
extern SemaphoreHandle_t xSwitchMutex;
/////////////////////////////////////////

void vLoadControlTask(void *pvParameters)
{   
    while(1)
    {
    }
}