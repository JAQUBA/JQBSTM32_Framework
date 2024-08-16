#ifndef __PWM_H_
#define __PWM_H_

#include "../../Core.h"

class PWM {
    public:
        PWM(TIM_HandleTypeDef *instance, uint32_t channel);
        void set(uint32_t value);
        uint32_t get();
    private:
        TIM_HandleTypeDef *_pHandler;
        uint32_t _channel;
        uint32_t _value;
};

#endif