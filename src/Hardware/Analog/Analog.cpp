#include "Analog.h"
#ifdef HAL_ADC_MODULE_ENABLED

uint16_t Analog::rawADC[8];

void Analog::init(ADC_HandleTypeDef *pHandler) {
    HAL_ADC_Start_DMA(pHandler, (uint32_t*)rawADC, pHandler->Init.NbrOfConversion);
}
Analog::Analog(uint8_t channelNumber) {
    _channelNumber = channelNumber;
}
void Analog::configureChannel(uint16_t *offset, uint16_t *divider) {
    _offset = offset;
    _divider = divider;
}
uint16_t Analog::getValue() {
    //return (uint16_t)(((rawADC[_channelNumber] * ((*_divider)*2)) >> 12) - (*_offset) );
    //return rawADC[_channelNumber];
    return (uint16_t)(((rawADC[_channelNumber] * ((*_divider))) >> 14) - (*_offset) );
}
uint16_t Analog::getRaw() {
    // uint32_t vdda_voltage = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(rawADC[7], ADC_RESOLUTION_12B);
    // uint32_t measurement_voltage = __HAL_ADC_CALC_DATA_TO_VOLTAGE(vdda_voltage, rawADC[_channelNumber], ADC_RESOLUTION_12B);
    // return measurement_voltage;

    return rawADC[_channelNumber];
}
#endif