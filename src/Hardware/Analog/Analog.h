/*
 * JQBSTM32 Framework - Analog.h Header
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
#include "../../Core.h"
#ifdef HAL_ADC_MODULE_ENABLED
#include <adc.h>

#ifndef __ANALOG_H_
#define __ANALOG_H_

#ifndef ANALOG_MAX_INSTANCES
#define ANALOG_MAX_INSTANCES 1
#endif

#ifndef ANALOG_MAX_CHANNELS
#define ANALOG_MAX_CHANNELS 8
#endif

/**
 * @brief Universal ADC input class exposing raw DMA ADC readings
 * @details Provides raw analog samples and voltage conversion.
 *          Filtering, calibration, and unit scaling belong to
 *          higher-level application modules.
 */
class Analog {
public:
    /**
     * @brief Intrusive receiver for ADC conversion-complete notifications.
     * @note The receiver owns its list storage, so registration has no fixed
     *       capacity and requires no heap allocation.
     */
    class InterruptListener {
        friend class Analog;

    public:
        using Callback = void (*)(InterruptListener* listener, uint16_t* buffer);

        explicit InterruptListener(Callback callback)
            : _callback(callback), _next(nullptr) {
        }

        InterruptListener(const InterruptListener&) = delete;
        InterruptListener& operator=(const InterruptListener&) = delete;

    private:
        Callback _callback;
        InterruptListener* _next;
    };

    static Analog* getInstance(ADC_HandleTypeDef *pHandler);

    Analog(ADC_HandleTypeDef *pHandler, uint16_t vref = 3300);
    ~Analog();

    void convCpltCallback();
    /**
     * @brief Register a callback invoked from the ADC conversion complete interrupt context.
     * @note Keep the callback short and non-blocking.
        * @param listener Receiver embedded in the object that processes samples.
        * @return True if the listener was registered or was already present.
     */
        bool attachInterrupt(InterruptListener* listener);

    /**
     * @brief Unregister a previously attached interrupt callback.
        * @return True if the listener was removed.
     */
        bool detachInterrupt(InterruptListener* listener);

    /**
     * @brief Remove all interrupt callbacks.
     */
    void clearInterrupts();

    /**
     * @brief Get raw ADC value from DMA buffer
     */
    uint16_t getValue(uint8_t channel) const;

    /**
        * @brief Get voltage in millivolts (from raw value)
     */
    uint16_t getVoltage(uint8_t channel) const;

    uint8_t getChannelCount() const { return _channelCount; }
    uint32_t getMaxValue() const { return _maxAdcValue; }
    uint16_t getVref() const { return _vref; }

private:
    ADC_HandleTypeDef *_pHandler;
    uint16_t *_adcBuffer;

    uint16_t _vref;
    uint8_t _channelCount;
    uint32_t _maxAdcValue;
    InterruptListener* _interruptListeners;
};

#endif // __ANALOG_H_
#endif // HAL_ADC_MODULE_ENABLED
