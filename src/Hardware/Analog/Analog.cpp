#include "Analog.h"
#ifdef __ANALOG_H_

Analog *_Analog_instances[ANALOG_MAX_INSTANCES];
uint8_t _Analog_instancesNum = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {Analog::getInstance(hadc)->convCpltCallback();}

Analog* Analog::getInstance(ADC_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _Analog_instancesNum; i++) {
        if(_Analog_instances[i]->_pHandler->Instance == pHandler->Instance) return _Analog_instances[i];
    }
    return nullptr;
}

Analog::Analog(ADC_HandleTypeDef *pHandler) : _pHandler(pHandler), bufferSize(pHandler->Init.NbrOfConversion) {
    _Analog_instances[_Analog_instancesNum++] = this;


    adcBuffer = new uint32_t[bufferSize]();
    offsets = new uint16_t[bufferSize];
    multipliers = new uint16_t[bufferSize];

    if (HAL_ADC_Start_DMA(pHandler, adcBuffer, bufferSize) != HAL_OK) {
        Error_Handler();
    }
}
void Analog::configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier) {
    if (channel < bufferSize) {
        offsets[channel] = *offset;
        multipliers[channel] = *multiplier;
    }
}
uint16_t Analog::getValue(uint8_t channel) {
    if (channel < bufferSize) {
        HAL_ADC_Start(_pHandler);
        HAL_ADC_PollForConversion(_pHandler, 100);
        return HAL_ADC_GetValue(_pHandler);
    }
    return 0;
}
Analog::~Analog() {
    delete[] offsets;
    delete[] multipliers;
}
void Analog::convCpltCallback() {
    for (size_t i = 0; i < bufferSize; i++) {
        adcBuffer[i] = (adcBuffer[i] * multipliers[i]) + offsets[i];
    }
}




#endif