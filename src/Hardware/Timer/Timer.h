#ifndef __TIMER_H_
#define __TIMER_H_

#include "Core.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "TimerCount.h"

#ifndef TIMER_MAX_INSTANCES
#define TIMER_MAX_INSTANCES TIMERS_AVAILABLE
#endif

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

        void attachInterrupt(InterruptType interruptType, voidCallback_f callback);
        void handleInterrupt(InterruptType interruptType);
    protected:
        TIM_HandleTypeDef* _pHandler;
        std::list<std::pair<InterruptType, voidCallback_f>> _callbacks;
};

#endif // _TIMER_H
#endif