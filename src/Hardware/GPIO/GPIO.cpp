/*
 * JQBSTM32 Framework - GPIO.cpp Implementation
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 */
#include "GPIO.h"
#ifdef HAL_GPIO_MODULE_ENABLED

HardwareGPIO GPIO;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    GPIO._interruptCallback(GPIO_Pin);
}

static uint8_t getPortIndex(GPIO_TypeDef* GPIOx) {
    if (GPIOx == nullptr) {
        return 0xFFU;
    }
#ifdef GPIOA
    if (GPIOx == GPIOA) return 0U;
#endif
#ifdef GPIOB
    if (GPIOx == GPIOB) return 1U;
#endif
#ifdef GPIOC
    if (GPIOx == GPIOC) return 2U;
#endif
#ifdef GPIOD
    if (GPIOx == GPIOD) return 3U;
#endif
#ifdef GPIOE
    if (GPIOx == GPIOE) return 4U;
#endif
#ifdef GPIOF
    if (GPIOx == GPIOF) return 5U;
#endif
#ifdef GPIOG
    if (GPIOx == GPIOG) return 6U;
#endif
#ifdef GPIOH
    if (GPIOx == GPIOH) return 7U;
#endif
#ifdef GPIOI
    if (GPIOx == GPIOI) return 8U;
#endif
    return 0xFFU;
}

static uint8_t getPinIndex(uint16_t GPIO_Pin) {
    for (uint8_t i = 0U; i < 16U; i++) {
        if (GPIO_Pin == (uint16_t)(1U << i)) {
            return i;
        }
    }
    return 0xFFU;
}

static bool matchesExtiSource(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    const uint8_t pinIndex = getPinIndex(GPIO_Pin);
    const uint8_t portIndex = getPortIndex(GPIOx);
    if (pinIndex >= 16U || portIndex == 0xFFU) {
        return false;
    }

#if defined(SYSCFG)
    const uint32_t extiConfig = SYSCFG->EXTICR[pinIndex / 4U];
#elif defined(AFIO)
    const uint32_t extiConfig = AFIO->EXTICR[pinIndex / 4U];
#else
    return true;
#endif

    const uint32_t configuredPort = (extiConfig >> ((pinIndex % 4U) * 4U)) & 0x0FU;
    return configuredPort == portIndex;
}

static uint8_t getExtiTriggerMask(uint16_t GPIO_Pin) {
    uint8_t triggerMask = 0U;

#if defined(EXTI)
    uint32_t risingTriggerRegister = 0U;
    uint32_t fallingTriggerRegister = 0U;

    #if defined(EXTI_RTSR1_RT0)
    risingTriggerRegister = EXTI->RTSR1;
    #else
    risingTriggerRegister = EXTI->RTSR;
    #endif

    #if defined(EXTI_FTSR1_FT0)
    fallingTriggerRegister = EXTI->FTSR1;
    #else
    fallingTriggerRegister = EXTI->FTSR;
    #endif

    if ((risingTriggerRegister & GPIO_Pin) != 0U) {
        triggerMask |= RISING;
    }
    if ((fallingTriggerRegister & GPIO_Pin) != 0U) {
        triggerMask |= FALLING;
    }
#endif

    return triggerMask;
}

void HardwareGPIO::_interruptCallback(uint16_t GPIO_Pin) {
    uint32_t currentTime = millis();
    
    for (uint8_t i = 0; i < MAX_GPIO_INTERRUPTS; i++) {
        if (interrupts[i].active &&
            interrupts[i].GPIOx != nullptr &&
            interrupts[i].GPIO_Pin == GPIO_Pin &&
            matchesExtiSource(interrupts[i].GPIOx, interrupts[i].GPIO_Pin)) {
            if (!matchesInterruptMode(interrupts[i])) {
                continue;
            }

            interrupts[i].triggerCount++;
            interrupts[i].lastTriggerTime = currentTime;
            
            if (interrupts[i].callback) {
                interrupts[i].callback();
            }
            break; // Found and processed, exit loop
        }
    }
}

bool HardwareGPIO::attachInterrupt(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, voidCallback_f callback, uint8_t mode) {
    // Check if interrupt already exists for this pin
    uint8_t existingSlot = findInterruptSlot(GPIOx, GPIO_Pin);
    if (existingSlot < MAX_GPIO_INTERRUPTS) {
        // Update existing interrupt
        interrupts[existingSlot].callback = callback;
        interrupts[existingSlot].triggerMode = mode;
        interrupts[existingSlot].lastState = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
        return true;
    }

    for (uint8_t i = 0; i < MAX_GPIO_INTERRUPTS; i++) {
        if (interrupts[i].active && interrupts[i].GPIO_Pin == GPIO_Pin) {
            if (matchesExtiSource(GPIOx, GPIO_Pin)) {
                interrupts[i].GPIOx = GPIOx;
                interrupts[i].callback = callback;
                interrupts[i].triggerMode = mode;
                interrupts[i].lastState = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
                interrupts[i].triggerCount = 0;
                interrupts[i].lastTriggerTime = millis();
                return true;
            }
            return false;
        }
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
    interrupts[freeSlot].triggerMode = mode;
    interrupts[freeSlot].lastState = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
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
        interrupts[slot].triggerMode = CHANGE;
        interrupts[slot].lastState = GPIO_PIN_RESET;
        interrupts[slot].triggerCount = 0;
        interrupts[slot].lastTriggerTime = 0;
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

bool HardwareGPIO::matchesInterruptMode(interrupt& interruptConfig) {
    const uint8_t extiTriggerMask = getExtiTriggerMask(interruptConfig.GPIO_Pin);

    switch (interruptConfig.triggerMode) {
        case RISING:
            return extiTriggerMask == RISING;
        case FALLING:
            return extiTriggerMask == FALLING;
        case CHANGE:
        default:
            return extiTriggerMask != 0U;
    }
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
