#ifndef __PWM_H_
#define __PWM_H_

#include "../../Core.h"

class PWM {
    public:
        PWM(TIM_HandleTypeDef *instance, uint32_t channel);
        void set(uint32_t value);
    private:
        TIM_HandleTypeDef *_instance;
        uint32_t _channel;
};

#endif