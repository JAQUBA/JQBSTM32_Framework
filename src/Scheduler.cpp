/*
 * JQBSTM32 Framework - Task Scheduler Implementation
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

#include "Scheduler.hpp"

taskStruct Scheduler::addTask(const taskCallback_f &func, uint32_t delay, bool single, taskTime time) {
    taskStruct task;
    
    // Find free slot
    uint8_t slotIndex = findFreeSlot();
    if (slotIndex >= MAX_SCHEDULER_TASKS) {
        // No free slots available, return invalid task
        return task;
    }

    delay *= time;

    task.functionPointer = func;
    task.delay = delay;
    task._delay = delay;
    task._single = single;
    task._id = _taskNum++;
    task.active = true;
    task.executionCount = 0;

    // Store task in array
    tasks[slotIndex] = task;
    _activeTaskCount++;
    
    return task;
}

bool Scheduler::removeTask(uint16_t taskId) {
    for (uint8_t i = 0; i < MAX_SCHEDULER_TASKS; i++) {
        if (tasks[i].isValid() && tasks[i]._id == taskId) {
            tasks[i].active = false;
            tasks[i]._id = INVALID_TASK_ID;
            _activeTaskCount--;
            return true;
        }
    }
    return false;
}

void Scheduler::execute() {
    for (uint8_t i = 0; i < MAX_SCHEDULER_TASKS; i++) {
        taskStruct& task = tasks[i];
        
        if (!task.isValid()) {
            continue;
        }
        
        if (task.delay == 0) {
            // Execute task
            if (task.functionPointer) {
                task.functionPointer(&task);
                task.executionCount++;
            }
            
            // Handle task lifecycle
            if (task._single) {
                // Remove single-execution task
                task.active = false;
                task._id = INVALID_TASK_ID;
                _activeTaskCount--;
            } else {
                // Reset delay for periodic task
                task.delay = task._delay;
            }
        }
    }
}

void Scheduler::poll() {
    for (uint8_t i = 0; i < MAX_SCHEDULER_TASKS; i++) {
        taskStruct& task = tasks[i];
        
        if (task.isValid() && task.delay > 0) {
            task.delay--;
        }
    }
}

uint8_t Scheduler::getActiveTaskCount() const {
    return _activeTaskCount;
}

uint8_t Scheduler::findFreeSlot() {
    for (uint8_t i = 0; i < MAX_SCHEDULER_TASKS; i++) {
        if (!tasks[i].isValid()) {
            return i;
        }
    }
    return MAX_SCHEDULER_TASKS; // No free slot found
}

taskStruct* Scheduler::findTask(uint16_t taskId) {
    for (uint8_t i = 0; i < MAX_SCHEDULER_TASKS; i++) {
        if (tasks[i].isValid() && tasks[i]._id == taskId) {
            return &tasks[i];
        }
    }
    return nullptr;
}