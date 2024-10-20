#ifndef _TIMER_H
#define _TIMER_H

#include "Core.h"

class Timer {
    public:
        Timer(TIM_HandleTypeDef *htim);
        void attachInterrupt(voidCallback_f callback);
    private:
        TIM_HandleTypeDef htim;
};


#endif // _TIMER_H