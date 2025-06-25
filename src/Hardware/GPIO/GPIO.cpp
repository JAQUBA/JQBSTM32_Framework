/*
 * JQBSTM32 Framework - GPIO.cpp Implementation
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
#include "GPIO.h"
#ifdef HAL_GPIO_MODULE_ENABLED

HardwareGPIO GPIO;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    GPIO._interruptCallback(GPIO_Pin);
}

void HardwareGPIO::_interruptCallback(uint16_t GPIO_Pin) {
    uint32_t currentTime = millis();
    
    for (uint8_t i = 0; i < MAX_GPIO_INTERRUPTS; i++) {
        if (interrupts[i].active && interrupts[i].GPIO_Pin == GPIO_Pin) {
            interrupts[i].triggerCount++;
            interrupts[i].lastTriggerTime = currentTime;
            
            if (interrupts[i].callback) {
                interrupts[i].callback();
            }
            break; // Found and processed, exit loop
        }
    }
}

bool HardwareGPIO::attachInterrupt(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, voidCallback_f callback) {
    // Check if interrupt already exists for this pin
    uint8_t existingSlot = findInterruptSlot(GPIOx, GPIO_Pin);
    if (existingSlot < MAX_GPIO_INTERRUPTS) {
        // Update existing interrupt
        interrupts[existingSlot].callback = callback;
        return true;
    }
    
    // Find free slot for new interrupt
    uint8_t freeSlot = findFreeInterruptSlot();
    if (freeSlot >= MAX_GPIO_INTERRUPTS) {
        return false; // No free slots available
    }
    
    // Configure new interrupt
    interrupts[freeSlot].GPIOx = GPIOx;
    interrupts[freeSlot].GPIO_Pin = GPIO_Pin;
    interrupts[freeSlot].callback = callback;
    interrupts[freeSlot].active = true;
    interrupts[freeSlot].triggerCount = 0;
    interrupts[freeSlot].lastTriggerTime = millis();
    interruptCount++;
    
    return true;
}

bool HardwareGPIO::detachInterrupt(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    uint8_t slot = findInterruptSlot(GPIOx, GPIO_Pin);
    if (slot < MAX_GPIO_INTERRUPTS) {
        interrupts[slot].active = false;
        interrupts[slot].GPIOx = nullptr;
        interrupts[slot].GPIO_Pin = 0;
        interrupts[slot].callback = nullptr;
        interruptCount--;
        return true;
    }
    return false;
}

uint32_t HardwareGPIO::getInterruptCount(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    uint8_t slot = findInterruptSlot(GPIOx, GPIO_Pin);
    if (slot < MAX_GPIO_INTERRUPTS) {
        return interrupts[slot].triggerCount;
    }
    return 0;
}

uint8_t HardwareGPIO::findInterruptSlot(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    for (uint8_t i = 0; i < MAX_GPIO_INTERRUPTS; i++) {
        if (interrupts[i].active && 
            interrupts[i].GPIOx == GPIOx && 
            interrupts[i].GPIO_Pin == GPIO_Pin) {
            return i;
        }
    }
    return MAX_GPIO_INTERRUPTS; // Not found
}

uint8_t HardwareGPIO::findFreeInterruptSlot() {
    for (uint8_t i = 0; i < MAX_GPIO_INTERRUPTS; i++) {
        if (!interrupts[i].active) {
            return i;
        }
    }
    return MAX_GPIO_INTERRUPTS; // No free slot
}
void HardwareGPIO::setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode) {
    GPIO_InitTypeDef GPIO_InitStruct  = {0};

    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void HardwareGPIO::setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pullup) {
    GPIO_InitTypeDef GPIO_InitStruct  = {0};

    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pullup;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void HardwareGPIO::set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
}
void HardwareGPIO::toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}
GPIO_PinState HardwareGPIO::readInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}
GPIO_PinState HardwareGPIO::readOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    assert_param(IS_GPIO_PIN(GPIO_Pin));
    if ((GPIOx->ODR & GPIO_Pin) != 0x00u) return GPIO_PIN_SET;
    return GPIO_PIN_RESET;
}
#endif
