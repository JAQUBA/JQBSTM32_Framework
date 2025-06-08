/*
 * JQBSTM32 Framework - Task Scheduler Header
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

#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>
#include <functional>
#include <list>

/**
 * @brief Task definition macro
 * @details Macro for defining task functions with proper signature
 * @param x Task function name
 */
#define TASK(x) void x(taskStruct *task)

/**
 * @brief Task callback macro
 * @details Lambda expression macro for inline task callbacks
 */
#define taskCallback [&](taskStruct *task)

/**
 * @brief Task callback function type
 * @details Function pointer type for task callbacks
 */
using taskCallback_f = std::function<void(struct taskStruct *task)>;

/**
 * @brief Data callback macro
 * @details Lambda expression macro for data processing callbacks
 */
#define dataCallback [&](uint8_t *data, uint16_t size)

/**
 * @brief Data callback function type
 * @details Function pointer type for data processing callbacks
 */
using dataCallback_f = std::function<void(uint8_t *data, uint16_t size)>;

/**
 * @brief Void callback macro
 * @details Lambda expression macro for simple void callbacks
 */
#define voidCallback [&](void)

/**
 * @brief Void callback function type
 * @details Function pointer type for simple void callbacks
 */
using voidCallback_f = std::function<void(void)>;

/**
 * @brief Task structure
 * @details Structure containing task information and execution parameters
 */
struct taskStruct {
	taskCallback_f  functionPointer; ///< Function to execute
    volatile uint32_t _delay = 0;    ///< Original delay value
	volatile uint32_t delay = 0;     ///< Current delay counter
	bool _single = false;            ///< Single execution flag

    uint16_t _id = 0;                ///< Task unique identifier
};

/**
 * @brief Task scheduler class
 * @details Manages and executes scheduled tasks with configurable timing
 */
class Scheduler {
    public:
    
        /**
         * @brief Time multiplier enumeration
         * @details Defines time base multipliers for task scheduling
         */
        enum taskTime {
            MUL_100US = 1,    ///< 100 microsecond base
            MUL_1MS = 10,     ///< 1 millisecond base (default)
            MUL_10MS = 100,   ///< 10 millisecond base
            MUL_100MS = 1000, ///< 100 millisecond base
            MUL_1S = 10000    ///< 1 second base
        };

        /**
         * @brief Add task to scheduler
         * @details Adds a new task to the scheduler with specified timing parameters
         * @param func Task function to execute
         * @param delay Initial delay before first execution
         * @param single Execute task only once (default: false)
         * @param time Time base multiplier (default: MUL_1MS)
         * @return taskStruct Task structure with task information
         */
        taskStruct addTask(const taskCallback_f &func, uint32_t delay, bool single = false, taskTime time = MUL_1MS);
        
        /**
         * @brief Execute scheduled tasks
         * @details Executes all tasks that are ready to run
         */
        void execute();
        
        /**
         * @brief Poll scheduler
         * @details Updates task timers and prepares tasks for execution
         */
        void poll();
    private:
        uint16_t _taskNum = 0;    ///< Task counter for ID assignment
        std::list<taskStruct> tasks; ///< List of scheduled tasks

};
#endif