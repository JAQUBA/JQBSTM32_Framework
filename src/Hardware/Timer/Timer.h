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
#define TIMER_MAX_INSTANCES TIMERS_AVAILABLE ///< Maximum number of timer instances
#endif

// #define timerCallback [&](Timer *timer)
// using timerCallback_f = std::function<void(class Timer *timer)>;

/**
 * @brief Timer hardware abstraction class
 * @details Provides timer functionality with interrupt handling and period control
 */
class Timer {
    public:
        /**
         * @brief Timer constructor
         * @details Initializes timer instance with the specified HAL handler
         * @param pHandler Pointer to HAL timer handler
         */
        explicit Timer(TIM_HandleTypeDef *pHandler);
        
        /**
         * @brief Get timer instance
         * @details Returns existing timer instance or creates new one for the specified handler
         * @param pHandler Pointer to HAL timer handler
         * @return Timer* Pointer to timer instance
         */
        static Timer *getInstance(TIM_HandleTypeDef *pHandler);

        /**
         * @brief Timer interrupt types enumeration
         * @details Defines different types of timer interrupts that can be handled
         */
        enum InterruptType {
            PeriodElapsedCallback,           ///< Timer period elapsed interrupt
            PeriodElapsedHalfCpltCallback,   ///< Timer period half complete interrupt
            OC_DelayElapsedCallback,         ///< Output compare delay elapsed interrupt
            IC_CaptureCallback,              ///< Input capture interrupt
            IC_CaptureHalfCpltCallback,      ///< Input capture half complete interrupt
            PWM_PulseFinishedCallback,       ///< PWM pulse finished interrupt
            PWM_PulseFinishedHalfCpltCallback, ///< PWM pulse half finished interrupt
            TriggerCallback,                 ///< Timer trigger interrupt
            TriggerHalfCpltCallback,         ///< Timer trigger half complete interrupt
            ErrorCallback                    ///< Timer error interrupt
        };

        /**
         * @brief Attach interrupt callback
         * @details Registers a callback function for specific timer interrupt type
         * @param interruptType Type of interrupt to handle
         * @param callback Function to be called on interrupt
         */
        void attachInterrupt(InterruptType interruptType, voidCallback_f callback);
        
        /**
         * @brief Handle timer interrupt
         * @details Internal function called by HAL to handle timer interrupts
         * @param interruptType Type of interrupt that occurred
         */
        void handleInterrupt(InterruptType interruptType);

        /**
         * @brief Set timer period
         * @details Sets the auto-reload value for the timer
         * @param period Timer period value
         */
        void setPeriod(uint32_t period);
    protected:
        TIM_HandleTypeDef* _pHandler; ///< HAL timer handler pointer
        std::list<std::pair<InterruptType, voidCallback_f>> _callbacks; ///< List of interrupt callbacks
};

#endif // _TIMER_H
#endif
