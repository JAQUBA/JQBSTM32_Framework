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
#include "Scheduler.hpp"

// Struktura dla pary GPIO (port + pin)
struct GPIO_Pin {
    GPIO_TypeDef* port;
    uint16_t pin;
};

#ifndef __weak
#define __weak __attribute__((weak))
#endif

class Core {
    public:
        Core();
};

taskStruct addTaskMain(taskCallback_f functionPointer, uint32_t delay = 0, bool single = false, Scheduler::taskTime time = Scheduler::taskTime::MUL_1MS);
taskStruct addTaskInterrupt(taskCallback_f functionPointer, uint32_t delay = 0, bool single = false, Scheduler::taskTime time = Scheduler::taskTime::MUL_1MS);

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

extern void SystemClock_Config();

long map(long x, long in_min, long in_max, long out_min, long out_max);

#ifdef __cplusplus
}
#endif

#endif