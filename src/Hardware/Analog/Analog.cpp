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
    for (auto& callback : _callbacks) {
        if (callback) {
            callback();
        }
    }

}
void Analog::attachInterrupt(voidCallback_f callback) {
    if (callback) {
        _callbacks.push_back(callback);
    }
}

uint16_t Analog::getValue(uint8_t channel) {
    if (channel >= _channelCount) return 0;
    return _adcBuffer[channel];
}

uint16_t Analog::getVoltage(uint8_t channel) {
    return (uint16_t)((getValue(channel) * _vref) / _maxAdcValue);
}

#endif // __ANALOG_H_
