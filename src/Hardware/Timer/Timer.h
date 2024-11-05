#include "../../Core.h"
#ifdef HAL_TIM_MODULE_ENABLED

#ifndef __TIMER_H_
#define __TIMER_H_

#include "TimerCount.h"
#ifndef TIMER_MAX_INSTANCES
#define TIMER_MAX_INSTANCES TIMERS_AVAILABLE
#endif

#define timerCallback [&](Timer *timer)
using timerCallback_f = std::function<void(class Timer *timer)>;

class Timer {
    public:
        explicit Timer(TIM_HandleTypeDef *pHandler);
        static Timer *getInstance(TIM_HandleTypeDef *pHandler);

        enum InterruptType {
            PeriodElapsedCallback,
            PeriodElapsedHalfCpltCallback,
            OC_DelayElapsedCallback,
            IC_CaptureCallback,
            IC_CaptureHalfCpltCallback,
            PWM_PulseFinishedCallback,
            PWM_PulseFinishedHalfCpltCallback,
            TriggerCallback,
            TriggerHalfCpltCallback,
            ErrorCallback
        };

        void attachInterrupt(InterruptType interruptType, timerCallback_f callback);
        void handleInterrupt(InterruptType interruptType);

        void setPeriod(uint32_t period);
    protected:
        TIM_HandleTypeDef* _pHandler;
        std::list<std::pair<InterruptType, timerCallback_f>> _callbacks;
};

#endif // _TIMER_H
#endif