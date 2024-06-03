#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "Core.h"
#include <functional>

typedef void (*FunctionPtr)();

typedef struct FunctionNodeParams {

} FunctionNodeParams;

typedef struct FunctionNode {
    FunctionPtr funcPtr;           // Wskaźnik do funkcji globalnej lub statycznej
    void (*methodPtr)(void*);      // Wskaźnik do metody instancji klasy
    void* objPtr;                  // Wskaźnik do obiektu instancji
    bool isMethod;                 // Flaga określająca typ wskaźnika
    struct FunctionNode* next;     // Wskaźnik do następnego węzła
} FunctionNode;

typedef FunctionNode taskStruct;

class Scheduler {
    public:
        static FunctionNode* createTask();

        void addTask();
        void execute();
        void poll1ms();
    private:
        struct FunctionNode* tasks;

};

void addTaskMain(std::function<void(taskStruct *task)> functionPointer, uint32_t delay, bool single = false);
void addTaskInterrupt(std::function<void(taskStruct *task)> functionPointer, uint32_t delay, bool single = false);


#endif