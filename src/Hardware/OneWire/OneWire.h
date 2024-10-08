#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "Core.h"

#include "Interface/IBus.h"

class OneWire : public IBus {
    public:
        OneWire(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

        void transmit(
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        void receive(
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        void write(uint8_t data);
        uint8_t read();
    private:
        GPIO_TypeDef* GPIOx;
        uint16_t GPIO_Pin;
};

#endif // ONEWIRE_H