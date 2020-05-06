
#ifndef QUEUE_TYPES_H
#define QUEUE_TYPES_H

#ifdef __SIMULATION__
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#else
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#endif

typedef enum
{
	false,
	true
} bool;

typedef enum
{
	Frequency,
	Roc
} UpdateType;

typedef struct ShedRequest
{
	bool isShed;
	TickType_t timestamp;
} ShedRequest;


typedef struct FreqReading
{
	double freq;
	double RoC;
	TickType_t timestamp;
} FreqReading;

typedef struct AnalyserReading
{
	unsigned int count;
	TickType_t timestamp;
} AnalyserReading;

#endif /* QUEUE_TYPES_H */
