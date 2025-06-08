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

#define TASK(x) void x(taskStruct *task)

#define taskCallback [&](taskStruct *task)
using taskCallback_f = std::function<void(struct taskStruct *task)>;

#define dataCallback [&](uint8_t *data, uint16_t size)
using dataCallback_f = std::function<void(uint8_t *data, uint16_t size)>;

#define voidCallback [&](void)
using voidCallback_f = std::function<void(void)>;

struct taskStruct {
	taskCallback_f  functionPointer;
    volatile uint32_t _delay = 0;
	volatile uint32_t delay = 0;
	bool _single = false;

    uint16_t _id = 0;
};

class Scheduler {
    public:
    
        enum taskTime {
            MUL_100US = 1,
            MUL_1MS = 10,
            MUL_10MS = 100,
            MUL_100MS = 1000,
            MUL_1S = 10000
        };

        taskStruct addTask(const taskCallback_f &func, uint32_t delay, bool single = false, taskTime time = MUL_1MS);
        void execute();
        void poll();
    private:
        uint16_t _taskNum = 0;
        std::list<taskStruct> tasks;

};
#endif