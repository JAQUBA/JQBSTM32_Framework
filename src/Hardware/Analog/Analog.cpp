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

// Static variables for instance management
Analog *_Analog_instances[ANALOG_MAX_INSTANCES];
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

Analog::Analog(ADC_HandleTypeDef *pHandler) : 
    _pHandler(pHandler), 
    _channelCount(pHandler->Init.NbrOfConversion),
    _dataReady(false) {
    
    // Register this instance
    if (_Analog_instancesNum < ANALOG_MAX_INSTANCES) {
        _Analog_instances[_Analog_instancesNum++] = this;
    }
      // Initialize buffers and calibration
    for (uint8_t i = 0; i < ANALOG_MAX_CHANNELS; i++) {
        _adcBuffer[i] = 0;
        _offsets[i] = nullptr;        // Default: no calibration configured
        _multipliers[i] = nullptr;    // Default: no calibration configured
    }
    
    // Calibrate ADC
    HAL_ADCEx_Calibration_Start(_pHandler);
    
    // Start ADC with DMA
    start();
}

void Analog::convCpltCallback() {
    _dataReady = true;
}

uint16_t Analog::getRawValue(uint8_t channel) {
    if (channel < _channelCount && channel < ANALOG_MAX_CHANNELS) {
        return _adcBuffer[channel];
    }
    return 0;
}

uint16_t Analog::getValue(uint8_t channel) {
    if (channel < _channelCount && channel < ANALOG_MAX_CHANNELS) {
        uint16_t raw = _adcBuffer[channel];
        
        // Check if calibration is configured
        if (_offsets[channel] == nullptr || _multipliers[channel] == nullptr) {
            return raw; // Return raw value if no calibration
        }
        
        // Apply calibration with bit shift optimization (equivalent to /16384)
        // Formula: ((raw * multiplier) >> 14) - offset
        uint32_t scaled = ((uint32_t)raw * (*_multipliers[channel])) >> 10; 
        
        // Subtract offset with underflow protection
        if (scaled >= (*_offsets[channel])) {
            uint32_t calibrated = scaled - (*_offsets[channel]);
            // Clamp to valid range
            if (calibrated > 65535) calibrated = 65535;
            return (uint16_t)calibrated;
        } else {
            return 0; // Underflow protection
        }
    }
    return 0;
}

uint16_t Analog::getVoltage(uint8_t channel, uint16_t vref) {
    if (channel < _channelCount && channel < ANALOG_MAX_CHANNELS) {
        // Convert raw ADC value to voltage (before calibration)
        // For 10-bit ADC: (ADC_value * Vref) / 1023
        uint32_t voltage = ((uint32_t)_adcBuffer[channel] * vref) / 1023;
        return (uint16_t)voltage;
    }
    return 0;
}

void Analog::configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier) {
    if (channel < ANALOG_MAX_CHANNELS) {
        _offsets[channel] = offset;
        _multipliers[channel] = multiplier;
    }
}

void Analog::start() {
    if (HAL_ADC_Start_DMA(_pHandler, (uint32_t*)_adcBuffer, _channelCount) != HAL_OK) {
        Error_Handler();
    }
}

void Analog::stop() {
    HAL_ADC_Stop_DMA(_pHandler);
}

#endif // __ANALOG_H_
