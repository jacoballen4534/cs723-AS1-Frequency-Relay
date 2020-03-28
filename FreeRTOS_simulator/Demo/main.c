#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define mainREGION_1_SIZE	7001
#define mainREGION_2_SIZE	18105
#define mainREGION_3_SIZE	2807

// void initialize_heap(void) {
//	static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
//	const HeapRegion_t xHeapRegions[] =
//	{
//		/* Start address with dummy offsets						Size */
//		{ ucHeap + 1,											mainREGION_1_SIZE },
//		{ ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
//		{ ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
//		{ NULL, 0 }
//	};
//
//	/* Pass the array into vPortDefineHeapRegions(). */
//	vPortDefineHeapRegions( xHeapRegions );
//}

const TickType_t task1Delay = 50 / portTICK_PERIOD_MS;
const TickType_t task2Delay = 100 / portTICK_PERIOD_MS;
QueueHandle_t q;

void Task1() {
	int i = 0;
	while (1) {
		BaseType_t t = xQueueSend(q, (void *)&i, 1);
		if (t == pdTRUE) {
			i++;
		}
		vTaskDelay(task1Delay);
	}
}

void Task2() {
	int i = 0;
	while (1) {
		BaseType_t t = xQueueReceive(q, (void *)&i, 1);
		if (t == pdTRUE) {
			printf("Received %d\n", i);
			fflush(stdout);
		}
		vTaskDelay(task2Delay);
	}
}

void testError(BaseType_t xR, char* msg) {
	if(xR == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
		fprintf(stderr, "Could not allocate required memory for %s", msg);
		exit(1);
	}
}


int main(int argc, char **argv) {
	//initialize_heap();

	// Creating a queue of depth 10 for storing sizeof(int)
	q = xQueueCreate(10, sizeof(int));
	if (q == 0)
		fputs("Could not create a queue", stderr);

	BaseType_t xR;
	TaskHandle_t hTask1;
	// Creating a sending task
	xR = xTaskCreate(Task1,"T1",500,NULL,1,&hTask1);
	testError(xR, "Task1");

	TaskHandle_t hTask2;
	// Creating a receiving task
	xR = xTaskCreate(Task2,"T2",500,NULL,2,&hTask2);
	testError(xR, "Task2");

	// Starting the scheduler
	vTaskStartScheduler();

	// In case if the scheduler failed to start.
	for(;;);
}
