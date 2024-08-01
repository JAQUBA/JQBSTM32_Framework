#ifndef __PWM_H_
#define __PWM_H_

#include "../../Core.h"
#include "Application/Display-MS12/Display.h"

class PWM {
    public:
        PWM(TIM_HandleTypeDef *instance, uint32_t channel, Display led);
        void set(uint32_t value);
    private:
        TIM_HandleTypeDef *_pHandler;
        uint32_t _channel;
        uint32_t _value;
};

#endif