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
    _interruptListeners(nullptr),
    _pendingHead(0),
    _pendingTail(0),
    _pendingCount(0),
    _pendingBuffers{} {

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

    addTaskMain(Analog::dispatchPendingConversions);
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

    const uint8_t channelCount = (_channelCount > ANALOG_MAX_CHANNELS) ? ANALOG_MAX_CHANNELS : _channelCount;
    uint8_t targetIndex = _pendingTail;
    if (_pendingCount >= ANALOG_PENDING_CONVERSIONS) {
        _pendingHead = (uint8_t)((_pendingHead + 1U) % ANALOG_PENDING_CONVERSIONS);
        targetIndex = _pendingTail;
    } else {
        _pendingCount++;
    }

    for (uint8_t channel = 0U; channel < channelCount; channel++) {
        _pendingBuffers[targetIndex][channel] = _adcBuffer[channel];
    }
    _pendingTail = (uint8_t)((targetIndex + 1U) % ANALOG_PENDING_CONVERSIONS);
}

void Analog::dispatchPendingConversions(taskStruct *task) {
    (void)task;
    for (uint8_t i = 0; i < _Analog_instancesNum; i++) {
        if (_Analog_instances[i] != nullptr) {
            _Analog_instances[i]->notifyPendingConversions();
        }
    }
}

void Analog::notifyPendingConversions() {
    const uint8_t channelCount = (_channelCount > ANALOG_MAX_CHANNELS) ? ANALOG_MAX_CHANNELS : _channelCount;
    while (true) {
        uint16_t sampleBuffer[ANALOG_MAX_CHANNELS];

		__disable_irq();
        if (_pendingCount == 0U) {
			__enable_irq();
            break;
        }
        const uint8_t sampleIndex = _pendingHead;
        for (uint8_t channel = 0U; channel < channelCount; channel++) {
            sampleBuffer[channel] = _pendingBuffers[sampleIndex][channel];
        }
        _pendingHead = (uint8_t)((_pendingHead + 1U) % ANALOG_PENDING_CONVERSIONS);
        _pendingCount--;
		__enable_irq();

        __disable_irq();
        InterruptListener* listener = _interruptListeners;
        __enable_irq();
        while (listener != nullptr) {
            __disable_irq();
            InterruptListener* next = listener->_next;
            __enable_irq();
            if (listener->_callback != nullptr) {
                listener->_callback(listener, sampleBuffer);
            }
            listener = next;
        }
    }
}

bool Analog::attachInterrupt(InterruptListener* listener) {
    if (listener == nullptr || listener->_callback == nullptr) {
        return false;
    }

    __disable_irq();
    for (InterruptListener* current = _interruptListeners;
         current != nullptr;
         current = current->_next) {
        if (current == listener) {
            __enable_irq();
            return true;
        }
    }

    listener->_next = _interruptListeners;
    _interruptListeners = listener;
    __enable_irq();
    return true;
}

bool Analog::detachInterrupt(InterruptListener* listener) {
    if (listener == nullptr) {
        return false;
    }

    __disable_irq();
    InterruptListener** current = &_interruptListeners;
    while (*current != nullptr) {
        if (*current == listener) {
            *current = listener->_next;
            listener->_next = nullptr;
            __enable_irq();
            return true;
        }
        current = &((*current)->_next);
    }

    __enable_irq();
    return false;
}

void Analog::clearInterrupts() {
    __disable_irq();
    InterruptListener* listener = _interruptListeners;
    _interruptListeners = nullptr;
    __enable_irq();

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
