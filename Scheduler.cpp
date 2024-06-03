#include "Scheduler.h"



FunctionNode* Scheduler::createTask() {
    return nullptr;
}
void Scheduler::addTask() {

}

void Scheduler::execute() {

}
void Scheduler::poll1ms() {

}

void addTaskInterrupt(std::function<void(taskStruct *task)> functionPointer, uint32_t delay, bool single) {

}
void addTaskMain(std::function<void(taskStruct *task)> functionPointer, uint32_t delay, bool single) {

}