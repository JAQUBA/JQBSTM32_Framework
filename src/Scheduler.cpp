#include "Scheduler.h"

void Scheduler::addTask(taskCallback_f functionPointer, uint32_t delay, bool single) {
    taskStruct node;

    node.functionPointer = functionPointer;
    node.delay = delay;
    node._delay = delay;
    node._single = single;
    node._id = _taskNum++;

    functions.push_back(node);
}

void Scheduler::execute() {
    for(taskStruct &node : functions) {
        if(node._delay == 0) {
            node.functionPointer(&node);
            node._delay = node.delay;
            if(node._single) {
                functions.remove(node);
            }
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