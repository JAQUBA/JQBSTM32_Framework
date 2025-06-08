/*
 * JQBSTM32 Framework - GPIO.h Header
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
#ifdef HAL_GPIO_MODULE_ENABLED

#ifndef __GPIO_H_
#define __GPIO_H_

#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET

class HardwareGPIO {
  public:
    void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode);
    void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pullup);
    void set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
    void toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    GPIO_PinState readInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    GPIO_PinState readOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

    void attachInterrupt(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, voidCallback_f callback);
    void _interruptCallback(uint16_t GPIO_Pin);

    private:
      struct interrupt {
        GPIO_TypeDef* GPIOx;
        uint16_t GPIO_Pin;
        voidCallback_f callback;
      };
      std::list<interrupt> interrupts;
};

extern HardwareGPIO GPIO;

#endif
#endif
