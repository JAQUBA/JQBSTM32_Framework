/*
 * JQBSTM32 Framework - TMC2209.h Header
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
#include "../../Hardware/GPIO/GPIO.h"
#ifdef __GPIO_H_

#ifndef __TMC2209_H_
#define __TMC2209_H_

/**
 * @class TMC2209
 * @brief Driver class for TMC2209 stepper motor driver
 * @details This class provides interface for controlling TMC2209 stepper motor driver
 *          via GPIO pins. The TMC2209 is a high-performance stepper motor driver with
 *          UART interface and StealthChop2 technology. Supports microstepping, current
 *          control, and various motion control features.
 * @note Requires GPIO connections for STEP, DIR, EN, MS1, MS2, and limit switch pins
 */
class TMC2209 {
    public:
        /**
         * @brief Direction enumeration for stepper motor
         */
        enum Direction {
            STEPPER_RIGHT = GPIO_PIN_SET,   ///< Clockwise/right direction
            STEPPER_LEFT = GPIO_PIN_RESET   ///< Counter-clockwise/left direction
        };
        
        /**
         * @brief Handler type enumeration for callback events
         */
        enum Handler {
            STEPPER_ON_LIMIT,  ///< Limit switch reached event
            STEP               ///< Step execution event
        };

        /**
         * @brief Constructor for TMC2209 stepper driver
         * @param enablePort GPIO port for enable pin
         * @param enablePin GPIO pin for enable control
         * @param stepPort GPIO port for step pin
         * @param stepPin GPIO pin for step pulse
         * @param directionPort GPIO port for direction pin
         * @param directionPin GPIO pin for direction control
         * @param MS1Port GPIO port for microstep selection 1
         * @param MS1Pin GPIO pin for microstep selection 1
         * @param MS2Port GPIO port for microstep selection 2
         * @param MS2Pin GPIO pin for microstep selection 2
         * @param zeroPort GPIO port for zero/limit switch
         * @param zeroPin GPIO pin for zero/limit switch input
         * @details Initializes TMC2209 driver with all necessary GPIO pin assignments.
         *          Does not perform hardware initialization - call init() after construction.
         */
        TMC2209(GPIO_TypeDef* enablePort, uint16_t enablePin,
            GPIO_TypeDef* stepPort, uint16_t stepPin,
            GPIO_TypeDef* directionPort, uint16_t directionPin,
            GPIO_TypeDef* MS1Port, uint16_t MS1Pin,
            GPIO_TypeDef* MS2Port, uint16_t MS2Pin,
            GPIO_TypeDef* zeroPort, uint16_t zeroPin);
            
        /**
         * @brief Initialize TMC2209 hardware
         * @details Configures GPIO pins and sets initial state for stepper driver.
         *          Must be called before using any motor control functions.
         */
        void init();

        /**
         * @brief Execute specified number of steps
         * @param steps Number of steps to execute (default: 1, negative for reverse direction)
         * @details Executes the specified number of step pulses in current direction.
         *          Negative values automatically reverse direction for the move.
         * @note Each step pulse duration should be appropriate for motor specifications
         */
        void step(int16_t steps = 1);
        
        /**
         * @brief Move motor forward (single step)
         * @details Executes one step in forward direction (STEPPER_RIGHT)
         */
        void forward();
        
        /**
         * @brief Move motor backward (single step)
         * @details Executes one step in backward direction (STEPPER_LEFT)
         */
        void back();

        /**
         * @brief Reverse current direction
         * @details Toggles the current direction setting between forward and backward
         */
        void reverse();
        
        /**
         * @brief Toggle motor power enable/disable
         * @details Switches motor between enabled and disabled states
         */
        void togglePower();
        
        /**
         * @brief Enable motor power
         * @details Enables stepper motor by asserting enable pin (motor can move)
         */
        void enable();
        
        /**
         * @brief Disable motor power
         * @details Disables stepper motor by deasserting enable pin (motor freewheels)
         */
        void disable();
        
        /**
         * @brief Set motor direction
         * @param direction Direction to set (STEPPER_RIGHT or STEPPER_LEFT)
         * @details Sets the direction pin to control motor rotation direction
         */
        void direction(Direction direction);

        /**
         * @brief Get current motor direction
         * @return Current direction setting (STEPPER_RIGHT or STEPPER_LEFT)
         */
        Direction getDirection();
        
        /**
         * @brief Set step limit for automatic stopping
         * @param limit Maximum number of steps before triggering limit handler
         * @details Sets a step counter limit that triggers callback when reached
         */
        void setLimit(uint32_t limit);
        
        /**
         * @brief Set callback handler for specific events
         * @param handler Handler type (STEPPER_ON_LIMIT or STEP)
         * @param fnHandler Function pointer to callback function
         * @details Registers callback functions for limit reaching or step execution events
         */
        void setHandler(Handler handler, void (*fnHandler)(void));

        /**
         * @brief Set limit switch callback handler
         * @param fnHandler Function to call when limit switch is triggered
         * @details Convenience method for setting limit switch callback
         */
        void onLimit(void (*fnHandler)(void));

        /**
         * @brief Get current step limit setting
         * @return Current step limit value
         */
        uint32_t getLimit();

        /**
         * @brief Set callback for specific step count
         * @param steps Number of steps after which to trigger callback
         * @param fnHandler Function to call when step count is reached
         * @details Sets up callback to be triggered after executing specified number of steps
         */
        void onStep(uint32_t steps, void (*fnHandler)(void));

        uint32_t _limit;  ///< Current step limit setting
        uint32_t _steps;  ///< Current step counter
        
    private:
        GPIO_TypeDef* _enablePort; uint16_t _enablePin;       ///< Enable pin configuration
        GPIO_TypeDef* _stepPort; uint16_t _stepPin;           ///< Step pin configuration
        GPIO_TypeDef* _directionPort; uint16_t _directionPin; ///< Direction pin configuration
        GPIO_TypeDef* _MS1Port; uint16_t _MS1Pin;             ///< Microstep 1 pin configuration
        GPIO_TypeDef* _MS2Port; uint16_t _MS2Pin;             ///< Microstep 2 pin configuration
        GPIO_TypeDef* _zeroPort; uint16_t _zeroPin;           ///< Zero/limit switch pin configuration
        Direction _direction;                                  ///< Current direction setting

        std::list<void (*)()> _onStepHandlers;  ///< List of step event handlers
        void (* _handlers [2])() = {};          ///< Array of event handlers
        uint32_t _onStepLimit;                  ///< Step count for triggering callbacks
};

#endif
#endif
