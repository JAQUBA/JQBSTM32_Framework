#ifndef _TIMER_H
#define _TIMER_H

#include "Core.h"

class Timer {
    public:
        Timer(TIM_TypeDef *timerBase, uint8_t prescaler, uint8_t period);
    private:
        TIM_HandleTypeDef htim;
};


#endif // _TIMER_H