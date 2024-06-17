#ifndef __CORE_H_
#define __CORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "fl_bit.h"
#include <functional>
#include "Scheduler.h"

typedef const char *String;

class Core {
    public:
        Core();
};

void addTaskMain(std::function<void(struct taskStruct *task)> functionPointer, uint32_t delay, bool single = false);
void addTaskInterrupt(std::function<void(struct taskStruct *task)> functionPointer, uint32_t delay, bool single = false);

#ifdef __cplusplus
extern "C" {
#endif

void init();
void setup();
void loop();

extern unsigned long ulMillis;
unsigned long millis();
void delay(volatile uint32_t delay_ms);
void delay_us(volatile uint32_t delay_us);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
extern void SystemClock_Config();

long map(long x, long in_min, long in_max, long out_min, long out_max);

#ifdef __cplusplus
}
#endif

#endif