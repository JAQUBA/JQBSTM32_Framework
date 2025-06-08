/*
 * JQBSTM32 Framework - Analog.cpp Implementation
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "Analog.h"
#ifdef __ANALOG_H_

Analog *_Analog_instances[ANALOG_MAX_INSTANCES];
uint8_t _Analog_instancesNum = 0;

uint16_t Analog::adcRAW[8];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    Analog::getInstance(hadc)->convCpltCallback();
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    Analog::getInstance(hadc)->convHalfCpltCallback();
}

Analog* Analog::getInstance(ADC_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _Analog_instancesNum; i++) {
        if(_Analog_instances[i]->_pHandler->Instance == pHandler->Instance) 
            return _Analog_instances[i];
    }
    return nullptr;
}

Analog::Analog(ADC_HandleTypeDef *pHandler) : _pHandler(pHandler), bufferSize(pHandler->Init.NbrOfConversion) {
    _Analog_instances[_Analog_instancesNum++] = this;

    // Buffer initialization
    adcDMABuffer = new uint32_t[bufferSize * ADC_DMA_BUFFER_SIZE];
    adcSamples = new uint32_t*[bufferSize];
    adcAverage = new uint32_t[bufferSize];
    adcFiltered = new float[bufferSize];
    adcValue = new uint16_t[bufferSize];
    
    for(size_t i = 0; i < bufferSize; i++) {
        adcSamples[i] = new uint32_t[ADC_SAMPLES_COUNT];
    }

    // Calibration initialization
    offsets = new uint16_t[bufferSize];
    multipliers = new uint16_t[bufferSize];
    
    for(size_t i = 0; i < bufferSize; i++) {
        offsets[i] = 0;
        multipliers[i] = ADC_DEFAULT_MULTIPLIER;
        adcFiltered[i] = 0.0f;
        adcAverage[i] = 0;
        adcValue[i] = 0;
    }

    // Control variables initialization
    sampleIndex = 0;
    dataReady = false;

    // ADC calibration
    HAL_ADCEx_Calibration_Start(pHandler);

    // Start ADC with DMA
    if (HAL_ADC_Start_DMA(pHandler, adcDMABuffer, bufferSize * ADC_DMA_BUFFER_SIZE) != HAL_OK) {
        Error_Handler();
    }

    // Data processing task
    addTaskMain(taskCallback {
        if(dataReady) {
            processData();
            dataReady = false;
        }
    }, ADC_PROCESS_TASK_PERIOD);
}

void Analog::convCpltCallback() {
    processDMAData(adcDMABuffer + bufferSize);  // Second half of buffer
}

void Analog::convHalfCpltCallback() {
    processDMAData(adcDMABuffer);  // First half of buffer
}

void Analog::processDMAData(uint32_t* data) {
    for(size_t channel = 0; channel < bufferSize; channel++) {
        adcSamples[channel][sampleIndex] = data[channel];
    }
    
    sampleIndex = (sampleIndex + 1) % ADC_SAMPLES_COUNT;
    
    if(sampleIndex == 0) {
        dataReady = true;
    }
}

void Analog::processData() {
    for(size_t channel = 0; channel < bufferSize; channel++) {
        // Sample averaging
        uint32_t sum = 0;
        for(size_t i = 0; i < ADC_SAMPLES_COUNT; i++) {
            sum += adcSamples[channel][i];
        }
        adcAverage[channel] = sum / ADC_SAMPLES_COUNT;
        
        // Low-pass filtering
        adcFiltered[channel] = adcFiltered[channel] * (1.0f - ADC_FILTER_ALPHA) + 
                              adcAverage[channel] * ADC_FILTER_ALPHA;
        
        // Calibration and conversion to 16-bit
        int32_t calibrated = ((int32_t)adcFiltered[channel] - offsets[channel]) * multipliers[channel] / 1000;
        
        // Limit to 16-bit range
        if(calibrated < 0) calibrated = 0;
        if(calibrated > 65535) calibrated = 65535;
        
        adcValue[channel] = (uint16_t)calibrated;
    }
}

uint16_t Analog::getValue(uint8_t channel) {
    if (channel < bufferSize) {
        return adcValue[channel];
    }
    return 0;
}

uint16_t Analog::getRawValue(uint8_t channel) {
    if (channel < bufferSize) {
        return (uint16_t)adcAverage[channel];
    }
    return 0;
}

float Analog::getFilteredValue(uint8_t channel) {
    if (channel < bufferSize) {
        return adcFiltered[channel];
    }
    return 0.0f;
}

void Analog::configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier) {
    if (channel < bufferSize) {
        offsets[channel] = *offset;
        multipliers[channel] = *multiplier;
    }
}

bool Analog::isDataReady() {
    return dataReady;
}

void Analog::resetFilters() {
    for(size_t i = 0; i < bufferSize; i++) {
        adcFiltered[i] = 0.0f;
    }
}

#endif // __ANALOG_H_
