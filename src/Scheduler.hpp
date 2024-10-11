#ifndef __SCHEDULER_H
#define __SCHEDULER_H

// #include "Core.h"
#include <functional>
#include <list>

#define taskCallback [&](taskStruct *task)
#define voidCallback [&](void)
#define TASK(x) void x(taskStruct *task)

using taskCallback_f = std::function<void(struct taskStruct *task)>;
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