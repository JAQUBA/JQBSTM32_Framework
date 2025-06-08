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

class TMC2209 {
    public:

        enum Direction {
            STEPPER_RIGHT = GPIO_PIN_SET,
            STEPPER_LEFT = GPIO_PIN_RESET
        };
        enum Handler {
            STEPPER_ON_LIMIT,
            STEP
        };

        TMC2209(GPIO_TypeDef* enablePort, uint16_t enablePin,
            GPIO_TypeDef* stepPort, uint16_t stepPin,
            GPIO_TypeDef* directionPort, uint16_t directionPin,
            GPIO_TypeDef* MS1Port, uint16_t MS1Pin,
            GPIO_TypeDef* MS2Port, uint16_t MS2Pin,
            GPIO_TypeDef* zeroPort, uint16_t zeroPin);
        void init();

        void step(int16_t steps = 1);
        void forward();
        void back();

        void reverse();
        void togglePower();
        void enable();
        void disable();
        void direction(Direction direction);

        Direction getDirection();
        
        void setLimit(uint32_t limit);
        void setHandler(Handler handler, void (*fnHandler)(void));

        

        void onLimit(void (*fnHandler)(void));

        uint32_t getLimit();


        void onStep(uint32_t steps, void (*fnHandler)(void));


        uint32_t _limit;
        uint32_t _steps;
    private:
        GPIO_TypeDef* _enablePort; uint16_t _enablePin;
        GPIO_TypeDef* _stepPort; uint16_t _stepPin;
        GPIO_TypeDef* _directionPort; uint16_t _directionPin;
        GPIO_TypeDef* _MS1Port; uint16_t _MS1Pin;
        GPIO_TypeDef* _MS2Port; uint16_t _MS2Pin;
        GPIO_TypeDef* _zeroPort; uint16_t _zeroPin;
        Direction _direction;

        std::list<void (*)()> _onStepHandlers;
        
        void (* _handlers [2])() = {};

        uint32_t _onStepLimit;
};

#endif
#endif
