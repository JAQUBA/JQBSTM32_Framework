/*
 * JQBSTM32 Framework - Core Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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

/**
 * @brief GPIO Pin structure for pairing port and pin
 * @details Structure containing GPIO port and pin information for easy GPIO handling
 */
struct GPIO_Pin {
    GPIO_TypeDef* port; ///< GPIO port pointer
    uint16_t pin;       ///< GPIO pin number
};

#ifndef __weak
#define __weak __attribute__((weak))
#endif

/**
 * @brief Core framework class
 * @details Main framework initialization class providing basic system setup
 */
class Core {
    public:
        /**
         * @brief Core constructor
         * @details Initializes the core framework components
         */
        Core();
};

/**
 * @brief Add task to main scheduler
 * @details Adds a new task to the main scheduler queue for execution in main loop context
 * @param functionPointer Function pointer to the task callback
 * @param delay Initial delay before first execution (default: 0)
 * @param single Execute task only once (default: false)
 * @param time Time multiplier for delay calculation (default: MUL_1MS)
 * @return taskStruct Task structure containing task information
 */
taskStruct addTaskMain(taskCallback_f functionPointer, uint32_t delay = 0, bool single = false, Scheduler::taskTime time = Scheduler::taskTime::MUL_1MS);

/**
 * @brief Add task to interrupt scheduler
 * @details Adds a new task to the interrupt scheduler for execution in interrupt context
 * @param functionPointer Function pointer to the task callback
 * @param delay Initial delay before first execution (default: 0)
 * @param single Execute task only once (default: false)
 * @param time Time multiplier for delay calculation (default: MUL_1MS)
 * @return taskStruct Task structure containing task information
 */
taskStruct addTaskInterrupt(taskCallback_f functionPointer, uint32_t delay = 0, bool single = false, Scheduler::taskTime time = Scheduler::taskTime::MUL_1MS);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize framework components
 * @details Initializes all framework components and hardware abstractions
 */
void init();

/**
 * @brief User setup function
 * @details User-defined setup function called once at startup (weak function)
 */
void setup();

/**
 * @brief User main loop function
 * @details User-defined main loop function called repeatedly (weak function)
 */
void loop();

extern volatile uint32_t ulMillis; ///< Global millisecond counter

/**
 * @brief Get current millisecond count
 * @details Returns the number of milliseconds since system startup
 * @return uint32_t Current millisecond count
 */
uint32_t millis();

/**
 * @brief Delay execution for specified milliseconds
 * @details Blocks execution for the specified number of milliseconds
 * @param delay_ms Delay time in milliseconds
 */
void delay(volatile uint32_t delay_ms);

/**
 * @brief Delay execution for specified microseconds
 * @details Blocks execution for the specified number of microseconds
 * @param delay_us Delay time in microseconds
 */
void delay_us(volatile uint32_t delay_us);

/**
 * @brief System clock configuration function
 * @details External function for configuring system clock (implemented by user)
 */
extern void SystemClock_Config();

/**
 * @brief Map value from one range to another
 * @details Re-maps a number from one range to another (Arduino-compatible function)
 * @param x The number to map
 * @param in_min The lower bound of the value's current range
 * @param in_max The upper bound of the value's current range
 * @param out_min The lower bound of the value's target range
 * @param out_max The upper bound of the value's target range
 * @return long The mapped value
 */
long map(long x, long in_min, long in_max, long out_min, long out_max);

/**
 * @brief Get system uptime in milliseconds
 * @details Returns the time elapsed since system initialization
 * @return uint32_t System uptime in milliseconds
 */
uint32_t getSystemUptime();

/**
 * @brief Get number of task overruns
 * @details Returns the count of times task execution exceeded threshold
 * @return uint32_t Number of task overruns
 */
uint32_t getTaskOverruns();

/**
 * @brief Get maximum execution time recorded
 * @details Returns the longest task execution time measured
 * @return uint32_t Maximum execution time in milliseconds
 */
uint32_t getMaxExecutionTime();

/**
 * @brief Reset performance counters
 * @details Resets all performance monitoring counters to zero
 */
void resetPerformanceCounters();

#ifdef __cplusplus
}
#endif

#endif