#include "Scheduler.h"

#include "Hardware/GPIO/GPIO.h"
#include "portMap.h"

void Scheduler::addTask(std::function<void(taskStruct *task)> functionPointer, uint32_t delay, bool single) {
    taskStruct node;

    node.functionPointer = functionPointer;
    node.delay = delay;
    node._delay = delay;
    node._single = single;

    functions.push_back(node);
}

void Scheduler::execute() {
    for(taskStruct &node : functions) {
        if(node._delay == 0) {
            node.functionPointer(&node);
            node._delay = node.delay;
        }
    }
}
void Scheduler::poll1ms() {
    for(taskStruct &node : functions) {
        if(node._delay > 0) {
            node._delay--;
        }
    }
}