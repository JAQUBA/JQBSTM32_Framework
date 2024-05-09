#ifndef __ANALOG_H
#define __ANALOG_H

#include "../../Core.h"
#include "main.h"

class Analog {
    public:
        static void init(ADC_HandleTypeDef *analog);
        Analog(uint8_t channelNumber);
        uint16_t getRaw();
        uint16_t getValue();
        void configureChannel(uint16_t *offset, uint16_t *divider);
    private:
        static uint16_t rawADC[8];
        uint8_t _channelNumber;
        uint16_t *_offset;
        uint16_t *_divider;
};

#endif