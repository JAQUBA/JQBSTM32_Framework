#ifndef GPIO_H_
#define GPIO_H_

#include "../../Core.h"

#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET

class HardwareGPIO {
  public:
    HardwareGPIO();
    void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode);
    void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pullup);
    void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pullup, uint32_t alternate);
    void set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
    void toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    GPIO_PinState readInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    GPIO_PinState readOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

};

extern HardwareGPIO GPIO;

#endif