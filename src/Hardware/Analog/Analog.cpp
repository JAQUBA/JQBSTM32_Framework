#include "Analog.h"
#ifdef __ANALOG_H_

uint16_t Analog::rawADC[8];

void Analog::init(ADC_HandleTypeDef *pHandler) {
    HAL_ADC_Start_DMA(pHandler, (uint32_t*)rawADC, pHandler->Init.NbrOfConversion);
}
Analog::Analog(uint8_t channelNumber) {
    _channelNumber = channelNumber;
}
void Analog::configureChannel(uint16_t *offset, uint16_t *multiplier) {
    _offset = offset;
    _multiplier = multiplier;
}
uint16_t Analog::getValue() {
    int16_t val =(uint16_t)(((rawADC[_channelNumber] * (*_multiplier)) >> 10) - (*_offset));
    if(val < 0) val = 0;
    return val;
}
#endif