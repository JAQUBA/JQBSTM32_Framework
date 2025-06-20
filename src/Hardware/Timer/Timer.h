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

#ifndef TIMER_MAX_CALLBACKS
#define TIMER_MAX_CALLBACKS 8 ///< Maximum number of callbacks per timer
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
        };        /**
         * @brief Attach interrupt callback
         * @details Registers a callback function for specific timer interrupt type
         * @param interruptType Type of interrupt to handle
         * @param callback Function to be called on interrupt
         * @return bool True if callback was successfully attached, false if no free slots
         */
        bool attachInterrupt(InterruptType interruptType, voidCallback_f callback);
        
        /**
         * @brief Detach interrupt callback
         * @details Removes callback function for specific timer interrupt type
         * @param interruptType Type of interrupt to remove
         */
        void detachInterrupt(InterruptType interruptType);
        
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
        
        /**
         * @brief Get number of attached callbacks
         * @details Returns the current number of attached interrupt callbacks
         * @return uint8_t Number of callbacks
         */
        uint8_t getCallbackCount() const;
    protected:
        /**
         * @brief Timer callback entry structure
         * @details Holds interrupt type and callback function pair
         */
        struct CallbackEntry {
            InterruptType type;
            voidCallback_f callback;
            bool active;
            
            CallbackEntry() : type(PeriodElapsedCallback), callback(nullptr), active(false) {}
        };
        
        TIM_HandleTypeDef* _pHandler; ///< HAL timer handler pointer
        CallbackEntry _callbacks[TIMER_MAX_CALLBACKS]; ///< Array of interrupt callbacks
        uint8_t _callbackCount; ///< Number of active callbacks
        
        /**
         * @brief Find free callback slot
         * @details Searches for an available slot in the callbacks array
         * @return int8_t Index of free slot or -1 if none available
         */
        int8_t findFreeCallbackSlot();
        
        /**
         * @brief Find callback by type
         * @details Searches for callback with specific interrupt type
         * @param interruptType Type of interrupt to find
         * @return int8_t Index of callback or -1 if not found
         */
        int8_t findCallback(InterruptType interruptType);
};

#endif // _TIMER_H
#endif
