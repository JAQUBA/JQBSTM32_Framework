#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "../../Core.h"
using callback_function_t = std::function<void(void)>;

#define ENCODER_MAX_INSTANCES 2

class Encoder {
    public:
        Encoder(TIM_HandleTypeDef *pHandler);
        static Encoder *getInstance(TIM_HandleTypeDef *pHandler);

        void init();
        
        bool getDirection();

        int32_t getValue();
        void setValue(int32_t value);

        void setLimits(int32_t min, int32_t max);

        void attachInterrupt(callback_function_t callback);
        void timInterrupt();

    private:
        TIM_HandleTypeDef *_pHandler;

        int32_t _value;
        int32_t _min = 0;
        int32_t _max = -1;
        
        callback_function_t fnCallback;
};

#endif