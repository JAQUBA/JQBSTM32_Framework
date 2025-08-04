/*
 * JQBSTM32 Framework - Analog.cpp AVR-style Implementation
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
#include <string.h>
#ifdef __ANALOG_H_

Analog* _Analog_instances[ANALOG_MAX_INSTANCES];
uint8_t _Analog_instancesNum = 0;

// HAL callback functions
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    Analog* instance = Analog::getInstance(hadc);
    if (instance != nullptr) {
        instance->convCpltCallback();
    }
}

Analog* Analog::getInstance(ADC_HandleTypeDef *pHandler) {
    const ADC_TypeDef* targetInstance = pHandler->Instance;
    for (uint8_t i = 0; i < _Analog_instancesNum; i++) {
        if(_Analog_instances[i]->_pHandler->Instance == targetInstance) {
            return _Analog_instances[i];
        }
    }
    return nullptr;
}

Analog::Analog(ADC_HandleTypeDef *pHandler, uint16_t vref) : 
    _pHandler(pHandler), 
    _channelCount(pHandler->Init.NbrOfConversion),
    _vref(vref) {
    
    if (_Analog_instancesNum < ANALOG_MAX_INSTANCES) {
        _Analog_instances[_Analog_instancesNum++] = this;
    }

    // Wyzeruj wszystkie kanały jedną operacją
    memset(_channels, 0, sizeof(_channels));

    // Zoptymalizowane obliczanie _maxAdcValue - bezpośrednie przypisanie
    switch (_pHandler->Init.Resolution) {
        case ADC_RESOLUTION_6B:   _maxAdcValue = 63; break;
        case ADC_RESOLUTION_8B:   _maxAdcValue = 255; break;
        case ADC_RESOLUTION_10B:  _maxAdcValue = 1023; break;
        case ADC_RESOLUTION_12B:  _maxAdcValue = 4095; break;
        #ifdef ADC_RESOLUTION_14B
        case ADC_RESOLUTION_14B:  _maxAdcValue = 16383; break;
        #endif
        #ifdef ADC_RESOLUTION_16B
        case ADC_RESOLUTION_16B:  _maxAdcValue = 65535; break;
        #endif
        default:                  _maxAdcValue = 4095; break;  // 12-bit default
    }
    
    HAL_ADCEx_Calibration_Start(_pHandler);
    
    if (HAL_ADC_Start_DMA(_pHandler, (uint32_t*)_adcBuffer, _channelCount) != HAL_OK) {
        Error_Handler();
    }
}
Analog::~Analog() {
    HAL_ADC_Stop_DMA(_pHandler);
}

void Analog::convCpltCallback() {
    const uint8_t channels = (_channelCount < ANALOG_MAX_CHANNELS) ? _channelCount : ANALOG_MAX_CHANNELS;
    
    for (uint8_t ch = 0; ch < channels; ch++) {
        ChannelData& channelData = _channels[ch];
        const uint16_t sample = _adcBuffer[ch];

        // Szybki algorytm moving average z przesunięciami bitowymi
        if (channelData.filterIndex < ANALOG_FILTER_SIZE) {
            // Faza inicjalizacji - dodawanie próbek
            channelData.filterSum += sample;
            channelData.filterIndex++;
            channelData.filteredValue = channelData.filterSum >> ANALOG_FILTER_SHIFT; // Dzielenie przez przesunięcie
        } else {
            // Faza ustabilizowana - sliding window average
            // Odejmij najstarszą próbkę i dodaj nową (aproksymacja)
            channelData.filterSum = channelData.filterSum - (channelData.filteredValue) + sample;
            channelData.filteredValue = channelData.filterSum >> ANALOG_FILTER_SHIFT; // Dzielenie przez przesunięcie
        }
    }
}

uint16_t Analog::getVoltage(uint8_t channel) {
    if (channel >= _channelCount) return 0;
    const ChannelData& channelData = _channels[channel];
    
    // Optymalizacja: użyj przesunięć bitowych dla popularnych rozdzielczości
    uint32_t voltage;
    if (_maxAdcValue == 4095) {        // 12-bit ADC
        voltage = (channelData.filteredValue * _vref) >> 12;
    } else if (_maxAdcValue == 1023) { // 10-bit ADC  
        voltage = (channelData.filteredValue * _vref) >> 10;
    } else if (_maxAdcValue == 255) {  // 8-bit ADC
        voltage = (channelData.filteredValue * _vref) >> 8;
    } else {
        // Fallback dla innych rozdzielczości
        voltage = (channelData.filteredValue * _vref) / _maxAdcValue;
    }
    
    return (uint16_t)voltage;
}

#endif // __ANALOG_H_
