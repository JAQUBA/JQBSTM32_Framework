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
    static Analog* getInstance(ADC_HandleTypeDef *pHandler);
    
    /**
     * @brief Analog constructor
     * @param pHandler Pointer to HAL ADC handler
     */
    Analog(ADC_HandleTypeDef *pHandler);
    
    /**
     * @brief ADC conversion complete callback
     */
    void convCpltCallback();
    
    /**
     * @brief Get raw ADC value for channel
     * @param channel ADC channel number (0-7)
     * @return uint16_t Raw ADC value (0-1023 for 10-bit ADC)
     */
    uint16_t getRawValue(uint8_t channel);
    
    /**
     * @brief Get calibrated ADC value for channel
     * @param channel ADC channel number (0-7)
     * @return uint16_t Calibrated value after offset and multiplier
     */
    uint16_t getValue(uint8_t channel);
    
    /**
     * @brief Get voltage for channel
     * @param channel ADC channel number (0-7)
     * @param vref Reference voltage in millivolts (default 3300mV)
     * @return uint16_t Voltage in millivolts
     */
    uint16_t getVoltage(uint8_t channel, uint16_t vref = 3300);
      /**
     * @brief Configure channel calibration
     * @param channel ADC channel number (0-7)
     * @param offset Pointer to offset value (dynamically updated, subtracted from raw ADC)
     * @param multiplier Pointer to multiplier value (dynamically updated, multiplied by 1000, e.g., 1000 = 1.0x)
     */
    void configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier);
    
    /**
     * @brief Start ADC conversion
     */
    void start();
    
    /**
     * @brief Stop ADC conversion
     */
    void stop();

private:
    ADC_HandleTypeDef *_pHandler;    ///< HAL ADC handler pointer
    uint8_t _channelCount;           ///< Number of configured channels
    uint16_t _adcBuffer[ANALOG_MAX_CHANNELS]; ///< DMA buffer for ADC values
    volatile bool _dataReady;        ///< Data ready flag
      // Calibration parameters
    uint16_t *_offsets[ANALOG_MAX_CHANNELS];     ///< Pointers to offset values for each channel
    uint16_t *_multipliers[ANALOG_MAX_CHANNELS]; ///< Pointers to multiplier values for each channel (x1000)
};

#endif // __ANALOG_H_
#endif // HAL_ADC_MODULE_ENABLED
