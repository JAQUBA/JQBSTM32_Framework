#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "Core.h"

class OneWire {
    public:
        OneWire(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
        void reset();
        void writeBit(uint8_t bit);
        uint8_t readBit();
        void writeByte(uint8_t byte);
        uint8_t readByte();

    private:
        GPIO_TypeDef* GPIOx;
        uint16_t GPIO_Pin;

        void setPinOutput();
        void setPinInput();
        void delayMicroseconds(uint32_t us);
};

#endif // ONEWIRE_H