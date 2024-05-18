#ifndef __CORE_H_
#define __CORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "fl_bit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef const char *String;

extern void SystemClock_Config();

#define ABS(X) ((X) > 0 ? (X) : -(X))
#define XOR(A,B) (!A != !B)

long map(long x, long in_min, long in_max, long out_min, long out_max);

struct taskStruct {
	void (*functionPointer)(struct taskStruct *task);
	struct taskStruct *next = NULL;
	volatile uint32_t _delay = 0;
	volatile uint32_t delay = 0;
	bool _single = false;
};

typedef uint32_t task;


task *addTaskMain(void (*functionPointer)(struct taskStruct *task), uint32_t delay, bool single = false);
task *addTaskInterrupt(void (*functionPointer)(struct taskStruct *task), uint32_t delay, bool single = false);

void removeTaskMain(task *task);

void init();
void setup();
void loop();

void delay(volatile uint32_t delay_ms);
void delay_us(volatile uint32_t delay_us);
extern unsigned long ulMillis;
unsigned long millis();

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif