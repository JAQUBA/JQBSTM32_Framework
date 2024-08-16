#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "../../Core.h"

using encoderCallback_f = std::function<void(void)>;

#ifndef ENCODER_MAX_INSTANCES
#define ENCODER_MAX_INSTANCES 2
#endif

class Encoder {
    public:
        enum StartType {
            START_POLL = 0,
            START_IT = 1,
            START_DMA = 2
        };
        Encoder(TIM_HandleTypeDef *pHandler, StartType startType = START_IT);
        static Encoder *getInstance(TIM_HandleTypeDef *pHandler);
        
        bool getDirection();

        int32_t getValue();
        void setValue(int32_t value);

        void setLimits(int32_t min, int32_t max);

        void attachInterrupt(encoderCallback_f callback);
        void timInterrupt();

    private:
        TIM_HandleTypeDef *_pHandler;

        int32_t _value;
        int32_t _min = 0;
        int32_t _max = -1;
        
        encoderCallback_f fnCallback = nullptr;
};

#endif