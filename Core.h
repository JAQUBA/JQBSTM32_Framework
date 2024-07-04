#ifndef __CORE_H_
#define __CORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <functional>
#include <list>
#include <queue>
#include "main.h"
#include "fl_bit.h"
#include "Scheduler.h"

using taskCallback_f = std::function<void(struct taskStruct *task)>;
using voidCallback_f = std::function<void(void)>;

class Core {
    public:
        Core();
};

void addTaskMain(taskCallback_f functionPointer, uint32_t delay = 0, bool single = false);
void addTaskInterrupt(taskCallback_f functionPointer, uint32_t delay = 0, bool single = false);

#ifdef __cplusplus
extern "C" {
#endif

void init();
void setup();
void loop();

extern uint32_t ulMillis;
uint32_t millis();
void delay(volatile uint32_t delay_ms);
void delay_us(volatile uint32_t delay_us);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
extern void SystemClock_Config();

long map(long x, long in_min, long in_max, long out_min, long out_max);

#ifdef __cplusplus
}
#endif

#endif