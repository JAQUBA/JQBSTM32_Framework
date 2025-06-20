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
#include <array>

// Configuration: Maximum number of tasks - can be adjusted based on memory requirements
#ifndef MAX_SCHEDULER_TASKS
#define MAX_SCHEDULER_TASKS 16
#endif

#ifndef INVALID_TASK_ID
#define INVALID_TASK_ID 0xFFFF
#endif

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
    uint16_t _id = INVALID_TASK_ID;  ///< Task unique identifier
    
    // Enhanced features for optimization
    bool active = false;             ///< Task active flag
    uint32_t executionCount = 0;     ///< Number of times executed
    
    // Constructor for easier initialization
    taskStruct() = default;
    
    // Method to check if task is valid
    inline bool isValid() const { 
        return _id != INVALID_TASK_ID && active; 
    }
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
         * @brief Remove task from scheduler
         * @details Removes task with specified ID from scheduler
         * @param taskId ID of task to remove
         * @return true if task was found and removed
         */
        bool removeTask(uint16_t taskId);
        
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
        
        /**
         * @brief Get number of active tasks
         * @return Number of active tasks
         */
        uint8_t getActiveTaskCount() const;
        
    private:
        uint16_t _taskNum = 1;    ///< Task counter for ID assignment (start from 1)
        uint8_t _activeTaskCount = 0;  ///< Number of active tasks
        std::array<taskStruct, MAX_SCHEDULER_TASKS> tasks; ///< Fixed-size array of tasks
        
        /**
         * @brief Find free task slot
         * @return Index of free slot or MAX_SCHEDULER_TASKS if none available
         */
        uint8_t findFreeSlot();
        
        /**
         * @brief Find task by ID
         * @param taskId Task ID to search for
         * @return Pointer to task or nullptr if not found
         */
        taskStruct* findTask(uint16_t taskId);

};
#endif