#ifndef _TIMER_H
#define _TIMER_H

#include "Core.h"

class Timer {
    public:
        Timer(TIM_HandleTypeDef *htim);
        void period(uint32_t period);
        void delay_us(uint32_t period);
        void attachInterrupt(voidCallback_f callback);
    private:
        static Timer *_instance;
        TIM_HandleTypeDef *_htim;
};


#endif // _TIMER_H