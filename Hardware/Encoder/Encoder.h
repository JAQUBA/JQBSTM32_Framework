#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "../../Core.h"

#define ENCODER_MAX_INSTANCES 2

class Encoder {
    public:
        Encoder(TIM_HandleTypeDef *instance);
        static Encoder *getInstance(TIM_HandleTypeDef *instance);

        void init();
        
        bool getDirection();

        int32_t getValue();
        void setValue(int32_t value);

        void setLimits(int32_t min, int32_t max);

        void attachInterrupt(std::function<void(void)> callback);
        void timInterrupt();

    private:
        TIM_HandleTypeDef *_pInstance;

        int32_t _value;
        int32_t _min = 0;
        int32_t _max = -1;
        
        std::function<void(void)> fnCallback;
};

#endif