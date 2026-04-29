/*
 * JQBSTM32 Framework - Analog.h Header
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
#include "../../Core.h"
#ifdef HAL_ADC_MODULE_ENABLED
#include <list>
#include <adc.h>

#ifndef __ANALOG_H_
#define __ANALOG_H_

#ifndef ANALOG_MAX_INSTANCES
#define ANALOG_MAX_INSTANCES 1
#endif

#ifndef ANALOG_MAX_CHANNELS
#define ANALOG_MAX_CHANNELS 8
#endif

#ifndef ANALOG_DEFAULT_FILTER_SHIFT
#define ANALOG_DEFAULT_FILTER_SHIFT 3
#endif

/**
 * @brief Universal ADC input class with per-channel EMA filtering
 * @details Provides raw and filtered analog readings via DMA.
 *          No application-level calibration — offset/multiplier belongs
 *          to the application layer.
 */
class Analog {
public:
    static Analog* getInstance(ADC_HandleTypeDef *pHandler);

    Analog(ADC_HandleTypeDef *pHandler, uint16_t vref = 3300);
    ~Analog();

    void convCpltCallback();
    void attachInterrupt(std::function<void(uint16_t*)> callback);

    /**
     * @brief Set EMA filter shift for all channels
     * @param shift Filter shift (1-8). Filter factor = 1/(1<<shift). Higher = smoother/slower.
     */
    void setFilterShift(uint8_t shift);

    /**
     * @brief Set EMA filter shift for specific channel
     */
    void setFilterShift(uint8_t channel, uint8_t shift);

    /**
     * @brief Get raw ADC value from DMA buffer (unfiltered)
     */
    uint16_t getRawValue(uint8_t channel);

    /**
     * @brief Get EMA-filtered ADC value
     */
    uint16_t getFilteredValue(uint8_t channel);

    /**
     * @brief Get voltage in millivolts (from filtered value)
     */
    uint16_t getVoltage(uint8_t channel);

    uint8_t getChannelCount() const { return _channelCount; }
    uint32_t getMaxValue() const { return _maxAdcValue; }
    uint16_t getVref() const { return _vref; }

private:
    struct ChannelFilter {
        uint32_t accumulator;
        uint8_t shift;
        bool initialized;
    };

    ADC_HandleTypeDef *_pHandler;
    uint16_t *_adcBuffer;
    ChannelFilter _filters[ANALOG_MAX_CHANNELS];

    uint16_t _vref;
    uint8_t _channelCount;
    uint32_t _maxAdcValue;

    std::list<std::function<void(uint16_t*)>> _callbacks;
};

#endif // __ANALOG_H_
#endif // HAL_ADC_MODULE_ENABLED
