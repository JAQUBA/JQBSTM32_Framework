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

void Analog::configureChannel(uint8_t channel, uint16_t *offset, uint16_t *divider) {
    if (channel < ANALOG_MAX_CHANNELS) {
        _channels[channel].offset = offset;
        _channels[channel].divider = divider;
    }
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
    
    for (uint8_t ch = 0; ch < channels; ++ch) {
        ChannelData& cd = _channels[ch];
        const uint16_t sample = _adcBuffer[ch];
        const uint8_t idx = cd.filterIndex;

        // Zaktualizuj sumę: odejmij starą, dodaj nową próbkę
        cd.filterSum += sample - cd.filterBuffer[idx];
        cd.filterBuffer[idx] = sample;

        // Przesuń indeks cyklicznie (optymalizacja modulo)
        const uint8_t nextIdx = idx + 1;
        cd.filterIndex = (nextIdx == ANALOG_FILTER_SIZE) ? 0 : nextIdx;

        // Ustaw gotowość filtra po pełnym cyklu
        if (!cd.filterReady && cd.filterIndex == 0) {
            cd.filterReady = true;
        }
    }
}

uint16_t Analog::getRawValue(uint8_t channel) {
    return (channel < _channelCount) ? _adcBuffer[channel] : 0;
}

uint16_t Analog::getVoltage(uint8_t channel) {
    if (channel >= _channelCount) return 0;
    
    const ChannelData& channelData = _channels[channel];
    uint16_t adcValue = channelData.filterReady 
        ? (uint16_t)(channelData.filterSum / ANALOG_FILTER_SIZE)
        : _adcBuffer[channel];
    return (uint16_t)(((uint32_t)adcValue * _vref) / _maxAdcValue);
}

uint16_t Analog::getValue(uint8_t channel) {
    if (channel >= _channelCount) return 0;
    
    const ChannelData& channelData = _channels[channel];
    
    // Sprawdź czy kalibracja jest skonfigurowana
    if (!channelData.divider || !channelData.offset) {
        return channelData.filterReady 
            ? (uint16_t)(channelData.filterSum / ANALOG_FILTER_SIZE)
            : _adcBuffer[channel];
    }
    
    uint16_t adcValue = channelData.filterReady 
        ? (uint16_t)(channelData.filterSum / ANALOG_FILTER_SIZE)
        : _adcBuffer[channel];
    
    // Zastosuj skalowanie (dzielenie przez 1024 jako przesunięcie bitowe)
    uint32_t scaled = ((uint32_t)adcValue * (*channelData.divider)) >> 10;
    
    // Zastosuj offset z zabezpieczeniem przed underflow
    uint32_t offset = *channelData.offset;
    return (scaled >= offset) ? (uint16_t)(scaled - offset) : 0;
}


#endif // __ANALOG_H_
