#ifndef __GPIO_H_
#define __GPIO_H_

#include "../../Core.h"

#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET

class HardwareGPIO {
  public:
    HardwareGPIO();
    void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode);
    void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pullup);
    // void setup(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pullup, uint32_t alternate);
    void set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
    void toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    GPIO_PinState readInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    GPIO_PinState readOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

    void attachInterrupt(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, voidCallback_f callback, uint32_t mode);
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