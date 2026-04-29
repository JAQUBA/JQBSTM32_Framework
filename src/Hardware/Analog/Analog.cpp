/*
 * JQBSTM32 Framework - Analog.cpp
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
    for (uint8_t i = 0; i < _Analog_instancesNum; i++) {
        if(_Analog_instances[i]->_pHandler->Instance == pHandler->Instance) {
            return _Analog_instances[i];
        }
    }
    return nullptr;
}

Analog::Analog(ADC_HandleTypeDef *pHandler, uint16_t vref) : 
    _pHandler(pHandler), 
    _vref(vref),
    _channelCount(pHandler->Init.NbrOfConversion) {
    
    if (_Analog_instancesNum < ANALOG_MAX_INSTANCES) {
        _Analog_instances[_Analog_instancesNum++] = this;
    }

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
        default:                  _maxAdcValue = 4095; break;
    }

    for (uint8_t i = 0; i < ANALOG_MAX_CHANNELS; i++) {
        _filters[i] = { 0, ANALOG_DEFAULT_FILTER_SHIFT, false };
    }

    HAL_ADCEx_Calibration_Start(_pHandler);
    _adcBuffer = new uint16_t[_channelCount];
    
    if (HAL_ADC_Start_DMA(_pHandler, (uint32_t*)_adcBuffer, _channelCount) != HAL_OK) {
        Error_Handler();
    }
}
Analog::~Analog() {
    HAL_ADC_Stop_DMA(_pHandler);
}

void Analog::convCpltCallback() {
    for (uint8_t i = 0; i < _channelCount; i++) {
        ChannelFilter& f = _filters[i];
        uint16_t raw = _adcBuffer[i];

        if (!f.initialized) {
            f.accumulator = (uint32_t)raw << f.shift;
            f.initialized = true;
        } else {
            f.accumulator += (int32_t)raw - (int32_t)(f.accumulator >> f.shift);
        }
    }

    for (auto& callback : _callbacks) {
        if (callback) {
            callback(_adcBuffer);
        }
    }
}

void Analog::attachInterrupt(std::function<void(uint16_t*)> callback) {
    if (callback) {
        _callbacks.push_back(callback);
    }
}

void Analog::setFilterShift(uint8_t shift) {
    if (shift < 1) shift = 1;
    if (shift > 8) shift = 8;
    for (uint8_t i = 0; i < ANALOG_MAX_CHANNELS; i++) {
        _filters[i].shift = shift;
        _filters[i].accumulator = 0;
        _filters[i].initialized = false;
    }
}

void Analog::setFilterShift(uint8_t channel, uint8_t shift) {
    if (channel >= ANALOG_MAX_CHANNELS) return;
    if (shift < 1) shift = 1;
    if (shift > 8) shift = 8;
    _filters[channel].shift = shift;
    _filters[channel].accumulator = 0;
    _filters[channel].initialized = false;
}

uint16_t Analog::getRawValue(uint8_t channel) {
    return (channel < _channelCount) ? _adcBuffer[channel] : 0;
}

uint16_t Analog::getFilteredValue(uint8_t channel) {
    if (channel >= _channelCount) return 0;
    return (uint16_t)(_filters[channel].accumulator >> _filters[channel].shift);
}

uint16_t Analog::getVoltage(uint8_t channel) {
    uint16_t filtered = getFilteredValue(channel);
    return (uint16_t)((uint32_t)filtered * _vref / _maxAdcValue);
}

#endif // __ANALOG_H_
