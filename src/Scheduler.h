#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "Core.h"
#include <functional>
#include <list>

#define taskCallback [&](taskStruct *task)
#define TASK(x) void x(taskStruct *task)

using taskCallback_f = std::function<void(struct taskStruct *task)>;

struct taskStruct {
	taskCallback_f  functionPointer;
    volatile uint16_t _delay = 0;
	volatile uint16_t delay = 0;
	bool _single = false;

    uint16_t _id = 0;
};

class Scheduler {
    public:
        void addTask(const taskCallback_f &func, uint32_t delay, bool single = false);
        void execute();
        void poll1ms();
    private:
        uint16_t _taskNum = 0;
        std::list<taskStruct> tasks;

};
#endif