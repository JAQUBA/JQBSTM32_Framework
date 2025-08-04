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

#include <array>

// Configuration: Maximum number of GPIO interrupts
#ifndef MAX_GPIO_INTERRUPTS
#define MAX_GPIO_INTERRUPTS 16
#endif

#define HIGH GPIO_PIN_SET ///< Logic high state
#define LOW GPIO_PIN_RESET ///< Logic low state

/**
 * @brief Hardware GPIO abstraction class
 * @details Provides high-level GPIO operations including setup, read, write, and interrupt handling
 */
class HardwareGPIO {
  public:
    /**
     * @brief Constructor
     * @details Initializes GPIO hardware abstraction
     */
    HardwareGPIO() : interruptCount(0) {}
    
    /**
     * @brief Setup GPIO pin with specified mode
     * @details Configures a GPIO pin with the specified mode (INPUT, OUTPUT, etc.)
     * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
     * @param GPIO_Pin GPIO pin number (GPIO_PIN_0, GPIO_PIN_1, etc.)
     * @param mode GPIO mode (GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, etc.)
     */
    void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode);
    
    /**
     * @brief Setup GPIO pin with mode and pull-up/pull-down configuration
     * @details Configures a GPIO pin with specified mode and pull resistor configuration
     * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
     * @param GPIO_Pin GPIO pin number (GPIO_PIN_0, GPIO_PIN_1, etc.)
     * @param mode GPIO mode (GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, etc.)
     * @param pullup Pull resistor configuration (GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN)
     */
    void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pullup);
    
    /**
     * @brief Set GPIO pin state
     * @details Sets the specified GPIO pin to HIGH or LOW state
     * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
     * @param GPIO_Pin GPIO pin number (GPIO_PIN_0, GPIO_PIN_1, etc.)
     * @param PinState Pin state (GPIO_PIN_SET or GPIO_PIN_RESET)
     */
    void set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
    
    /**
     * @brief Toggle GPIO pin state
     * @details Toggles the current state of the specified GPIO pin
     * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
     * @param GPIO_Pin GPIO pin number (GPIO_PIN_0, GPIO_PIN_1, etc.)
     */
    void toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    
    /**
     * @brief Read GPIO input pin state
     * @details Reads the current state of the specified GPIO input pin
     * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
     * @param GPIO_Pin GPIO pin number (GPIO_PIN_0, GPIO_PIN_1, etc.)
     * @return GPIO_PinState Current pin state (GPIO_PIN_SET or GPIO_PIN_RESET)
     */
    GPIO_PinState readInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    
    /**
     * @brief Read GPIO output pin state
     * @details Reads the current state of the specified GPIO output pin
     * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
     * @param GPIO_Pin GPIO pin number (GPIO_PIN_0, GPIO_PIN_1, etc.)
     * @return GPIO_PinState Current pin state (GPIO_PIN_SET or GPIO_PIN_RESET)
     */
    GPIO_PinState readOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

    /**
     * @brief Attach interrupt callback to GPIO pin
     * @details Registers a callback function to be called when an interrupt occurs on the specified pin
     * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
     * @param GPIO_Pin GPIO pin number (GPIO_PIN_0, GPIO_PIN_1, etc.)
     * @param callback Function to be called on interrupt
     * @return true if interrupt was attached successfully
     */
    bool attachInterrupt(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, voidCallback_f callback);
    
    /**
     * @brief Detach interrupt from GPIO pin
     * @details Removes interrupt callback from the specified pin
     * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
     * @param GPIO_Pin GPIO pin number (GPIO_PIN_0, GPIO_PIN_1, etc.)
     * @return true if interrupt was detached successfully
     */
    bool detachInterrupt(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    
    /**
     * @brief Get interrupt statistics
     * @details Returns number of times interrupt was triggered for specific pin
     * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
     * @param GPIO_Pin GPIO pin number (GPIO_PIN_0, GPIO_PIN_1, etc.)
     * @return Number of interrupt triggers
     */
    uint32_t getInterruptCount(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    
    /**
     * @brief Internal interrupt callback handler
     * @details Internal function called by HAL interrupt handler
     * @param GPIO_Pin GPIO pin number that triggered the interrupt
     */
    void _interruptCallback(uint16_t GPIO_Pin);

    private:
      /**
       * @brief Enhanced interrupt configuration structure
       * @details Structure holding interrupt configuration for a GPIO pin with statistics
       */
      struct interrupt {
        GPIO_TypeDef* GPIOx;     ///< GPIO port
        uint16_t GPIO_Pin;       ///< GPIO pin number
        voidCallback_f callback; ///< Callback function
        bool active;             ///< Interrupt active flag
        uint32_t triggerCount;   ///< Number of times triggered
        uint32_t lastTriggerTime;///< Last trigger timestamp
        
        interrupt() : GPIOx(nullptr), GPIO_Pin(0), active(false), 
                     triggerCount(0), lastTriggerTime(0) {}
      };
      
      std::array<interrupt, MAX_GPIO_INTERRUPTS> interrupts; ///< Fixed-size array of interrupts
      uint8_t interruptCount; ///< Number of active interrupts
      
      /**
       * @brief Find interrupt slot for given pin
       * @param GPIOx GPIO port
       * @param GPIO_Pin GPIO pin number
       * @return Index of interrupt slot or MAX_GPIO_INTERRUPTS if not found
       */
      uint8_t findInterruptSlot(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
      
      /**
       * @brief Find free interrupt slot
       * @return Index of free slot or MAX_GPIO_INTERRUPTS if none available
       */
      uint8_t findFreeInterruptSlot();
};

/**
 * @brief Global GPIO instance
 * @details Global instance of HardwareGPIO class for convenient access
 */
extern HardwareGPIO GPIO;

#endif
#endif
