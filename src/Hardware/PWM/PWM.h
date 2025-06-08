/*
 * JQBSTM32 Framework - PWM.h Header
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

#ifndef __PWM_H_
#define __PWM_H_

#include "../Timer/Timer.h"

/**
 * @brief PWM (Pulse Width Modulation) class
 * @details Provides PWM signal generation with configurable duty cycle
 * @note Inherits from Timer class for timer-based operations
 */
class PWM : public Timer {
    public:
        /**
         * @brief PWM constructor
         * @details Initializes PWM instance with the specified timer and channel
         * @param pHandler Pointer to HAL timer handler
         * @param channel Timer channel for PWM output (TIM_CHANNEL_1, TIM_CHANNEL_2, etc.)
         */
        PWM(TIM_HandleTypeDef *pHandler, uint32_t channel);
        
        /**
         * @brief Set PWM duty cycle value
         * @details Sets the PWM compare value (duty cycle)
         * @param value PWM compare value (0 to timer auto-reload value)
         */
        void set(uint32_t value);
        
        /**
         * @brief Get current PWM duty cycle value
         * @details Returns the current PWM compare value
         * @return uint32_t Current PWM compare value
         */
        uint32_t get();
    private:
        uint32_t _channel; ///< Timer channel used for PWM
        uint32_t _value;   ///< Current PWM value
};

#endif
#endif
