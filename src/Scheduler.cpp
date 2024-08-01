#include "Scheduler.h"

void Scheduler::addTask(const taskCallback_f &func, uint32_t delay, bool single) {
    taskStruct task;

    task.functionPointer = func;
    task.delay = delay;
    task._delay = delay;
    task._single = single;
    task._id = _taskNum++;

    tasks.push_back(task);
}

void Scheduler::execute() {
    for (auto it = tasks.begin(); it != tasks.end(); ) {
        if (it->delay == 0) {
            it->functionPointer(&(*it));
            it->delay = it->_delay;
            if (it->_single) {
                it = tasks.erase(it);
            }
        }
        ++it;
    }
}
void Scheduler::poll1ms() {
    for (auto &task : tasks) {
        if (task.delay > 0) {
            --task.delay;
        }
    }
}