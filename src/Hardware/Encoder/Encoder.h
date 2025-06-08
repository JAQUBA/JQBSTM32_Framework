/*
 * JQBSTM32 Framework - Encoder.h Header
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

#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "../Timer/Timer.h"

/**
 * @class Encoder
 * @brief Quadrature encoder interface class for rotary encoders
 * @details This class provides functionality for reading quadrature encoders using STM32 timer peripherals
 *          in encoder mode. Supports direction detection, value limits, and interrupt-based callbacks.
 *          Inherits from Timer class for low-level timer operations.
 * @note Requires timer peripheral configured in encoder mode with appropriate input channels
 */
class Encoder : public Timer {
    public:
        /**
         * @brief Encoder start mode enumeration
         * @details Defines how the encoder peripheral should be started and operated
         */
        enum StartType {
            START_POLL = 0,  ///< Polling mode - manual reading required
            START_IT = 1,    ///< Interrupt mode - automatic updates with callbacks
            START_DMA = 2    ///< DMA mode - hardware-assisted updates
        };
        
        /**
         * @brief Constructor for Encoder class
         * @param pHandler Pointer to STM32 HAL timer handle structure configured for encoder mode
         * @param channel Timer channels to use (default: TIM_CHANNEL_ALL for both channels)
         * @param startType Operating mode for encoder (default: START_IT for interrupt mode)
         * @details Initializes encoder with specified timer and starts operation in selected mode.
         *          Timer must be pre-configured for encoder mode with appropriate input filters.
         * @note Timer should be configured with TIM_ENCODERMODE_TI12 for quadrature decoding
         */
        Encoder(TIM_HandleTypeDef *pHandler, uint32_t channel = TIM_CHANNEL_ALL, StartType startType = START_IT);

        /**
         * @brief Get current rotation direction of encoder
         * @return true if encoder is rotating in forward/clockwise direction, false for reverse/counter-clockwise
         * @details Direction is determined by comparing current timer counter with previous reading.
         *          Forward direction typically corresponds to increasing counter values.
         * @note Direction detection requires encoder to be actively running
         */
        bool getDirection();

        /**
         * @brief Get current encoder position value
         * @return Current encoder count as signed 32-bit integer
         * @details Returns accumulated encoder counts since initialization or last reset.
         *          Value can be negative if encoder rotates in reverse direction.
         * @note Value is automatically constrained by limits if set via setLimits()
         */
        int32_t getValue();

        /**
         * @brief Set encoder position to specific value
         * @param value New encoder position value to set
         * @details Allows manual setting of encoder position for calibration or reference purposes.
         *          Value will be constrained by configured limits if applicable.
         * @note This directly modifies the internal counter value
         */
        void setValue(int32_t value);

        /**
         * @brief Configure value limits for encoder position
         * @param min Minimum allowed encoder value (inclusive)
         * @param max Maximum allowed encoder value (inclusive)
         * @details Sets boundaries for encoder values. When limits are active, encoder value
         *          will be constrained within [min, max] range. Use max = -1 to disable limits.
         * @note Default behavior has no limits (min=0, max=-1 indicates unlimited)
         */
        void setLimits(int32_t min, int32_t max);

        /**
         * @brief Attach interrupt callback function for encoder events
         * @param callback Function to call when encoder interrupt occurs
         * @details Registers callback function that will be executed on encoder-related interrupts
         *          such as overflow, underflow, or direction change events.
         * @note Callback function signature should be: void callback(void)
         * @note Only available when encoder is started in START_IT mode
         */
        void attachInterrupt(voidCallback_f callback);

    private:
        int32_t _value;     ///< Current encoder position value
        int32_t _min = 0;   ///< Minimum allowed encoder value (0 = no lower limit when max=-1)
        int32_t _max = -1;  ///< Maximum allowed encoder value (-1 = no upper limit)
        
        voidCallback_f fnCallback; ///< User callback function for encoder interrupts
};

#endif
#endif
