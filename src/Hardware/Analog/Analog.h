#include "../../Core.h"
#ifdef HAL_ADC_MODULE_ENABLED

#ifndef __ANALOG_H_
#define __ANALOG_H_

#ifndef ANALOG_MAX_INSTANCES
#define ANALOG_MAX_INSTANCES 1
#endif

#include "Application/Display/Display.h"
extern Display display;

class Analog {
    public:
        static Analog* getInstance(ADC_HandleTypeDef *pHandler);
        Analog(ADC_HandleTypeDef *pHandler);
        void configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier);

        uint16_t getValue(uint8_t channel);
        
        void convCpltCallback();
    private:
        ADC_HandleTypeDef *_pHandler;
        uint32_t bufferSize;
        static uint16_t adcRAW[8];

        uint32_t *adcBuffer;
        uint16_t adcSamplesNum;

        uint32_t *adcValue;

        uint16_t *offsets;
        uint16_t *multipliers;
};

#endif
#endif