#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include <altera_avalon_pio_regs.h>

extern unsigned int getsem_task1_got_sem;
extern unsigned int getsem_task2_got_sem;
extern char sem_owner_task_name[20];
extern SemaphoreHandle_t shared_resource_sem;

// The next two task compete for a shared resource via a semaphore.  The name of
// the task that owns the semaphore is copied into the global variable
// sem_owner_task_name[].
void getsem_task1(void *pvParameters)
{
	while (1)
	{
		xSemaphoreTake(shared_resource_sem, portMAX_DELAY);
		// block forever until receive the mutex
		strcpy(&sem_owner_task_name[0], "getsem_task1");
		getsem_task1_got_sem++;
		xSemaphoreGive(shared_resource_sem);
		vTaskDelay(100);
	}
}

void getsem_task2(void *pvParameters)
{
	while (1)
	{
		xSemaphoreTake(shared_resource_sem, portMAX_DELAY);
		// block forever until receive the mutex
		strcpy(&sem_owner_task_name[0], "getsem_task2");
		getsem_task2_got_sem++;
		xSemaphoreGive(shared_resource_sem);
		vTaskDelay(130);
	}
}
