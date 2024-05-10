#include "Analog.h"

uint16_t Analog::rawADC[8];

void Analog::init(ADC_HandleTypeDef *instance) {
    HAL_ADC_Start_DMA(instance, (uint32_t*)rawADC, instance->Init.NbrOfConversion);
}
Analog::Analog(uint8_t channelNumber) {
    _channelNumber = channelNumber;
}
void Analog::configureChannel(uint16_t *offset, uint16_t *divider) {
    _offset = offset;
    _divider = divider;
}
uint16_t Analog::getValue() {
    return (uint16_t)(((rawADC[_channelNumber] * ((*_divider)*10) ) >> 20) - (*_offset) );
}
uint16_t Analog::getRaw() {
    uint32_t vdda_voltage = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(rawADC[7], ADC_RESOLUTION_12B);
    uint32_t measurement_voltage = __HAL_ADC_CALC_DATA_TO_VOLTAGE(vdda_voltage, rawADC[_channelNumber], ADC_RESOLUTION_12B);
    return measurement_voltage;

    // return rawADC[_channelNumber];
}