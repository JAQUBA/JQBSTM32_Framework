/*
 * JQBSTM32 Framework - Analog.cpp
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 */
#include "Analog.h"
#ifdef __ANALOG_H_

#include <cstring>

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
    if (pHandler == nullptr) {
        return nullptr;
    }

    for (uint8_t i = 0; i < _Analog_instancesNum; i++) {
        if (_Analog_instances[i] != nullptr &&
            _Analog_instances[i]->_pHandler != nullptr &&
            _Analog_instances[i]->_pHandler->Instance == pHandler->Instance) {
            return _Analog_instances[i];
        }
    }
    return nullptr;
}

Analog::Analog(ADC_HandleTypeDef *pHandler, uint16_t vref) : 
    _pHandler(pHandler), 
    _adcBuffer(nullptr),
    _vref(vref),
    _channelCount(pHandler != nullptr ? pHandler->Init.NbrOfConversion : 0),
    _maxAdcValue(0),
    _interruptListeners(nullptr) {

    if (_pHandler == nullptr) {
        Error_Handler();
        return;
    }
    
    if (_Analog_instancesNum < ANALOG_MAX_INSTANCES) {
        _Analog_instances[_Analog_instancesNum++] = this;
    } else {
        Error_Handler();
        return;
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

    if (_channelCount > ANALOG_MAX_CHANNELS) {
        _channelCount = ANALOG_MAX_CHANNELS;
    }

    if (_channelCount == 0) {
        _adcBuffer = nullptr;
        return;
    }

    HAL_ADCEx_Calibration_Start(_pHandler);
    _adcBuffer = new uint16_t[_channelCount];
    
    if (HAL_ADC_Start_DMA(_pHandler, (uint32_t*)_adcBuffer, _channelCount) != HAL_OK) {
        Error_Handler();
    }
}
Analog::~Analog() {
    if (_pHandler != nullptr) {
        HAL_ADC_Stop_DMA(_pHandler);
    }

    delete[] _adcBuffer;
    _adcBuffer = nullptr;

    for (uint8_t i = 0; i < _Analog_instancesNum; i++) {
        if (_Analog_instances[i] == this) {
            if (i + 1 < _Analog_instancesNum) {
                std::memmove(&_Analog_instances[i],
                             &_Analog_instances[i + 1],
                             (_Analog_instancesNum - i - 1) * sizeof(_Analog_instances[0]));
            }
            _Analog_instances[--_Analog_instancesNum] = nullptr;
            break;
        }
    }
}

void Analog::convCpltCallback() {
    if (_adcBuffer == nullptr) {
        return;
    }

    InterruptListener* listener = _interruptListeners;
    while (listener != nullptr) {
        InterruptListener* next = listener->_next;
        if (listener->_callback != nullptr) {
            listener->_callback(listener, _adcBuffer);
        }
        listener = next;
    }
}

bool Analog::attachInterrupt(InterruptListener* listener) {
    if (listener == nullptr || listener->_callback == nullptr) {
        return false;
    }

    for (InterruptListener* current = _interruptListeners;
         current != nullptr;
         current = current->_next) {
        if (current == listener) {
            return true;
        }
    }

    listener->_next = _interruptListeners;
    _interruptListeners = listener;
    return true;
}

bool Analog::detachInterrupt(InterruptListener* listener) {
    if (listener == nullptr) {
        return false;
    }

    InterruptListener** current = &_interruptListeners;
    while (*current != nullptr) {
        if (*current == listener) {
            *current = listener->_next;
            listener->_next = nullptr;
            return true;
        }
        current = &((*current)->_next);
    }

    return false;
}

void Analog::clearInterrupts() {
    InterruptListener* listener = _interruptListeners;
    _interruptListeners = nullptr;

    while (listener != nullptr) {
        InterruptListener* next = listener->_next;
        listener->_next = nullptr;
        listener = next;
    }
}

uint16_t Analog::getValue(uint8_t channel) const {
    return (_adcBuffer != nullptr && channel < _channelCount) ? _adcBuffer[channel] : 0;
}

uint16_t Analog::getVoltage(uint8_t channel) const {
    if (_maxAdcValue == 0) {
        return 0;
    }

    uint16_t raw = getValue(channel);
    return (uint16_t)((uint32_t)raw * _vref / _maxAdcValue);
}

#endif // __ANALOG_H_
