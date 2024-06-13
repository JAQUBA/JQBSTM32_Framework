#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "Core.h"
#include <functional>
#include <list>

struct taskStruct {
	std::function<void(taskStruct *task)> functionPointer;
    volatile uint16_t _delay = 0;
	volatile uint16_t delay = 0;
	bool _single = false;
};

class Scheduler {
    public:
        void addTask(std::function<void(taskStruct *task)> functionPointer, uint32_t delay, bool single = false);
        void execute();
        void poll1ms();
    private:
        std::list<taskStruct> functions;

};
#endif