/*
 * JQBSTM32 Framework - TMC2209.cpp Implementation
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
#include "TMC2209.h"
#ifdef __TMC2209_H_

TMC2209::TMC2209(
        GPIO_TypeDef* enablePort, uint16_t enablePin,
        GPIO_TypeDef* stepPort, uint16_t stepPin,
        GPIO_TypeDef* directionPort, uint16_t directionPin,
        GPIO_TypeDef* MS1Port, uint16_t MS1Pin,
        GPIO_TypeDef* MS2Port, uint16_t MS2Pin,
        GPIO_TypeDef* zeroPort, uint16_t zeroPin
) {
    _enablePort = enablePort;
    _enablePin = enablePin;
    GPIO.setup(enablePort, enablePin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);

    _stepPort = stepPort;
    _stepPin = stepPin;
    GPIO.setup(stepPort, stepPin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);

    _directionPort = directionPort;
    _directionPin = directionPin;
    GPIO.setup(directionPort, directionPin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);
    GPIO.set(directionPort, directionPin, GPIO_PIN_SET);

    _MS1Port = MS1Port;
    _MS1Pin = MS1Pin;
    GPIO.setup(MS1Port, MS1Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);

    _MS2Port = MS2Port;
    _MS2Pin = MS2Pin;
    GPIO.setup(MS2Port, MS2Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);

    _zeroPort = zeroPort;
    _zeroPin = zeroPin;
    GPIO.setup(zeroPort, zeroPin, GPIO_MODE_INPUT, GPIO_PULLUP);
    _limit = -1;
}
void TMC2209::init() {
    bool lastState = GPIO.readInput(_zeroPort, _zeroPin);

    direction(lastState?STEPPER_LEFT:STEPPER_RIGHT);
    
    while(true) {
        if(GPIO.readInput(_zeroPort, _zeroPin) != lastState) break;
        step();
        delay(1);
    }
    direction(STEPPER_RIGHT);
    _steps = 0;
}
void TMC2209::step(int16_t steps) {
    for(uint16_t i = 0; i < ABS(steps) ; i++ ) {
        if(steps > 0) forward();
        else back();
    }

    if(_handlers[STEP]) _handlers[STEP]();
}
void TMC2209::forward() {
    if(++_steps == _limit) {
        _steps = 0;
        if(_handlers[STEPPER_ON_LIMIT]) _handlers[STEPPER_ON_LIMIT]();
        reverse();
    }
    GPIO.toggle(_stepPort, _stepPin);
}
void TMC2209::back() {
    if(--_steps == 0) {
        _steps = _limit;
        if(_handlers[STEPPER_ON_LIMIT]) _handlers[STEPPER_ON_LIMIT]();
        reverse();
    }
    GPIO.toggle(_stepPort, _stepPin);
}

void TMC2209::reverse() {
    if(_direction == STEPPER_LEFT) {
        _direction = STEPPER_RIGHT;
    }
    else {
        _direction = STEPPER_LEFT;
    }

    GPIO.toggle(_directionPort, _directionPin);
}
void TMC2209::direction(Direction direction) {
    _direction = direction;
    GPIO.set(_directionPort, _directionPin, (GPIO_PinState) direction);
}
TMC2209::Direction TMC2209::getDirection() {
    return _direction;
}

void TMC2209::togglePower() {
    GPIO.toggle(_enablePort, _enablePin);
}
void TMC2209::enable() {
    GPIO.set(_enablePort, _enablePin, GPIO_PIN_SET);
}
void TMC2209::disable() {
    GPIO.set(_enablePort, _enablePin, GPIO_PIN_RESET);
}

void TMC2209::setLimit(uint32_t limit) {
    _limit = limit;
}
void TMC2209::setHandler(Handler handler, void (*fnHandler)(void)) {
    _handlers[handler] = fnHandler;
}
void TMC2209::onLimit(void (*fnHandler)(void)) {
    _handlers[STEPPER_ON_LIMIT] = fnHandler;
}

uint32_t TMC2209::getLimit() {
    return _limit;
}

void TMC2209::onStep(uint32_t steps, void (*fnHandler)(void)) {
    _handlers[STEP] = fnHandler;
    _onStepLimit = steps;
}
#endif
