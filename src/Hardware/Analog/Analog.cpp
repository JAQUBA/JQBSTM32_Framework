#include "Analog.h"
#ifdef __ANALOG_H_

Analog *_Analog_instances[ANALOG_MAX_INSTANCES];
uint8_t _Analog_instancesNum = 0;

uint16_t Analog::adcRAW[8];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {Analog::getInstance(hadc)->convCpltCallback();}

Analog* Analog::getInstance(ADC_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _Analog_instancesNum; i++) {
        if(_Analog_instances[i]->_pHandler->Instance == pHandler->Instance) return _Analog_instances[i];
    }
    return nullptr;
}

Analog::Analog(ADC_HandleTypeDef *pHandler) : _pHandler(pHandler), bufferSize(pHandler->Init.NbrOfConversion) {
    _Analog_instances[_Analog_instancesNum++] = this;

    

    // adcRAW = new uint32_t[bufferSize];

    adcBuffer = new uint32_t[bufferSize];

    adcValue = new uint32_t[bufferSize];

    offsets = new uint16_t[bufferSize];
    multipliers = new uint16_t[bufferSize];

    HAL_ADCEx_Calibration_Start(pHandler);

    if (HAL_ADC_Start_DMA(pHandler, adcBuffer, bufferSize) != HAL_OK) {
        Error_Handler();
    }
    addTaskMain(taskCallback {
        for (size_t i = 0; i < bufferSize; i++) {
            // adcValue[i] = ((adcBuffer[i] / adcSamplesNum));
            // adcBuffer[i] = 0;
        }
        // adcSamplesNum = 0;
        display.toggle(displayMap::S3);//, true);
    }, 1000);
}
void Analog::convCpltCallback() {
    display.toggle(displayMap::S1);//, true);
    for (size_t i = 0; i < bufferSize; i++) {
        // adcBuffer[i] = adcRAW[i];
        // adcSamplesNum++;
    }
}

uint16_t Analog::getValue(uint8_t channel) {
    // if (channel < bufferSize) {
        return 200;
    // }
    // return 0;
}
void Analog::configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier) {
    if (channel < bufferSize) {
        offsets[channel] = *offset;
        multipliers[channel] = *multiplier;
    }
}
#endif