// Standard includes
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Scheduler includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <altera_avalon_pio_regs.h>

// Definition of Task Stacks
#define   TASK_STACKSIZE       2048

// Definition of Task Priorities
#define FREQ_ANALYSER_TASK_PRIORITY 14

// Local Function Prototypes
int initCreateTasks(void);

// Forward Declarations of ISRs
void freq_isr();

//Forward Declare Tasks
void process_frequency(void *pvParameters);

void init_isr()
{
	alt_irq_register(FREQUENCY_ANALYSER_IRQ, 0, freq_isr);
}

int main(int argc, char* argv[], char* envp[])
{
	init_isr();
	initCreateTasks();
	vTaskStartScheduler();
	for (;;);
	return 0;
}

// This function creates the tasks used in this example
int initCreateTasks(void)
{
	xTaskCreate(process_frequency, "process_frequency", TASK_STACKSIZE, NULL, FREQ_ANALYSER_TASK_PRIORITY, NULL);
	return 0;
}
