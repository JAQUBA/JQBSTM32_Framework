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

#ifndef __ANALOG_H_
#define __ANALOG_H_

#ifndef ANALOG_MAX_INSTANCES
#define ANALOG_MAX_INSTANCES 1 ///< Maximum number of ADC instances
#endif

#ifndef ANALOG_MAX_CHANNELS
#define ANALOG_MAX_CHANNELS 8 ///< Maximum number of ADC channels
#endif

#ifndef ANALOG_FILTER_SIZE
#define ANALOG_FILTER_SIZE 64 ///< Size of moving average filter buffer (power of 2 for bit shifting)
#endif

#ifndef ANALOG_FILTER_SHIFT
#define ANALOG_FILTER_SHIFT 6 ///< Bit shift for division by ANALOG_FILTER_SIZE (2^6 = 64)
#endif

/**
 * @brief Simple Analog/ADC input processing class with calibration
 * @details Provides analog input reading with DMA and channel calibration
 */
class Analog {
public:
    /**
     * @brief Get ADC instance
     * @param pHandler Pointer to HAL ADC handler
     * @return Analog* Pointer to ADC instance
     */
    static Analog* getInstance(ADC_HandleTypeDef *pHandler);    /**
     * @brief Analog constructor
     * @param pHandler Pointer to HAL ADC handler
     * @param vref Reference voltage in millivolts (default 3300mV)
     */
    Analog(ADC_HandleTypeDef *pHandler, uint16_t vref = 3300);
    
    /**
     * @brief Analog destructor - automatically stops ADC conversion
     */
    ~Analog();
    
    /**
     * @brief ADC conversion complete callback
     */
    void convCpltCallback();
    
    /**
     * @brief Get raw ADC value for channel
     * @param channel ADC channel number (0-7)
     * @return uint16_t Raw ADC value (0-1023 for 10-bit ADC)
     */
    inline uint16_t getRawValue(uint8_t channel) {
        return (channel < _channelCount) ? _adcBuffer[channel] : 0;
    }
    
    /**
     * @brief Get calibrated ADC value for channel
     * @param channel ADC channel number (0-7)
     * @return uint16_t Calibrated value after offset and multiplier
     */
    inline uint16_t getValue(uint8_t channel) {
        if (channel >= _channelCount) return 0;
        const ChannelData& channelData = _channels[channel];
        const uint16_t offset = channelData.offset ? *channelData.offset : 0;
        return (channelData.filteredValue >= offset) ? (uint16_t)(channelData.filteredValue - offset) : 0;
    }
    
    /**
     * @brief Get voltage for channel
     * @param channel ADC channel number (0-7)
     * @return uint16_t Voltage in millivolts
     */
    uint16_t getVoltage(uint8_t channel);

    /**
     * @brief Get maximum ADC value for current resolution
     * @return Maximum ADC value (cached for performance)
     */
    uint32_t getMaxValue() const { return _maxAdcValue; }
    
    /**
     * @brief Configure channel calibration
     * @param channel ADC channel number (0-7)
     * @param offset Pointer to offset value (dynamically updated, subtracted from raw ADC)
     * @param divider Pointer to divider value (dynamically updated, raw value is divided by this)
     */
    inline void configureChannel(uint8_t channel, uint16_t *offset, uint16_t *divider) {
        if (channel < ANALOG_MAX_CHANNELS) {
            _channels[channel].offset = offset;
            _channels[channel].divider = divider;
        }
    }
private:    /**
     * @brief Channel configuration and state structure (optimized)
     */
    struct ChannelData {
        uint16_t *offset;                                           ///< Pointer to offset value (dynamically updated)
        uint16_t *divider;                                          ///< Pointer to divider value (dynamically updated)

        uint32_t filterSum;                                         ///< Sum of values in buffer
        uint16_t filterIndex;                                       ///< Current index in circular buffer
        uint32_t filteredValue;                                     ///< Filtered value (cached) 
    };
    
    ADC_HandleTypeDef *_pHandler;    ///< HAL ADC handler pointer
    uint8_t _channelCount;           ///< Number of configured channels
    uint16_t _vref;                  ///< Reference voltage in millivolts
    uint32_t _maxAdcValue;           ///< Maximum ADC value based on resolution (cached)
    uint16_t _adcBuffer[ANALOG_MAX_CHANNELS]; ///< DMA buffer for ADC values
    
    ChannelData _channels[ANALOG_MAX_CHANNELS]; ///< Channel configuration and state data
};

#endif // __ANALOG_H_
#endif // HAL_ADC_MODULE_ENABLED
