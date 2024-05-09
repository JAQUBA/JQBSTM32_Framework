#ifndef __CORE_H
#define __CORE_H

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
	volatile uint16_t _delay = 0;
	volatile uint16_t delay = 0;
	bool _single = false;
};


void addTaskMain(void (*functionPointer)(struct taskStruct *task), uint16_t delay, bool single = false);
void addTaskInterrupt(void (*functionPointer)(struct taskStruct *task), uint16_t delay, bool single = false);

void init();
void setup();
void loop();

void delay(uint32_t delay_ms);
extern unsigned long ulMillis;
unsigned long millis();

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif