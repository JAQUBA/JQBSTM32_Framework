#ifndef __PWM_H_
#define __PWM_H_

#include "../../Core.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "../Timer/Timer.h"

class PWM : public Timer {
    public:
        PWM(TIM_HandleTypeDef *pHandler, uint32_t channel);
        void set(uint32_t value);
        uint32_t get();
    private:
        uint32_t _channel;
        uint32_t _value;
};

#endif
#endif