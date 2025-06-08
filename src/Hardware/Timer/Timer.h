/*
 * JQBSTM32 Framework - Timer.h Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "../../Core.h"
#ifdef HAL_TIM_MODULE_ENABLED

#ifndef __TIMER_H_
#define __TIMER_H_

#include "TimerCount.h"
#ifndef TIMER_MAX_INSTANCES
#define TIMER_MAX_INSTANCES TIMERS_AVAILABLE
#endif

// #define timerCallback [&](Timer *timer)
// using timerCallback_f = std::function<void(class Timer *timer)>;

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

        void setPeriod(uint32_t period);
    protected:
        TIM_HandleTypeDef* _pHandler;
        std::list<std::pair<InterruptType, voidCallback_f>> _callbacks;
};

#endif // _TIMER_H
#endif
