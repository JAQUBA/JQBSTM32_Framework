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

/**
 * @brief ADC configuration constants
 * @details Configuration parameters for analog input processing
 */
#ifndef ADC_SAMPLES_COUNT
#define ADC_SAMPLES_COUNT 64    ///< Number of samples for averaging
#endif

#ifndef ADC_FILTER_ALPHA
#define ADC_FILTER_ALPHA 0.1f   ///< Low-pass filter coefficient (0-1)
#endif

#ifndef ADC_DMA_BUFFER_SIZE
#define ADC_DMA_BUFFER_SIZE 2   ///< DMA buffer size (double buffer)
#endif

#ifndef ADC_DEFAULT_MULTIPLIER
#define ADC_DEFAULT_MULTIPLIER 1000  ///< Default multiplier (1.0 in fixed point)
#endif

#ifndef ADC_PROCESS_TASK_PERIOD
#define ADC_PROCESS_TASK_PERIOD 10   ///< Data processing task period [ms]
#endif

/**
 * @brief Analog/ADC input processing class
 * @details Provides analog input reading with filtering, averaging, and calibration
 */
class Analog {
public:
    /**
     * @brief Get ADC instance
     * @details Returns existing ADC instance or creates new one for the specified handler
     * @param pHandler Pointer to HAL ADC handler
     * @return Analog* Pointer to ADC instance
     */
    static Analog* getInstance(ADC_HandleTypeDef *pHandler);
    
    /**
     * @brief Analog constructor
     * @details Initializes ADC instance with the specified HAL handler
     * @param pHandler Pointer to HAL ADC handler
     */
    Analog(ADC_HandleTypeDef *pHandler);
    
    /**
     * @brief ADC conversion complete callback
     * @details Internal function called by HAL when ADC conversion is complete
     */
    void convCpltCallback();
    
    /**
     * @brief ADC conversion half complete callback
     * @details Internal function called by HAL when ADC conversion is half complete
     */
    void convHalfCpltCallback();
    
    /**
     * @brief Get processed ADC value
     * @details Returns calibrated and processed ADC value for specified channel
     * @param channel ADC channel number
     * @return uint16_t Processed ADC value
     */
    uint16_t getValue(uint8_t channel);
    
    /**
     * @brief Get raw ADC value
     * @details Returns unprocessed raw ADC value for specified channel
     * @param channel ADC channel number
     * @return uint16_t Raw ADC value
     */
    uint16_t getRawValue(uint8_t channel);
    
    /**
     * @brief Get filtered ADC value
     * @details Returns filtered ADC value as floating point for specified channel
     * @param channel ADC channel number
     * @return float Filtered ADC value
     */
    float getFilteredValue(uint8_t channel);
    
    /**
     * @brief Configure channel calibration
     * @details Sets offset and multiplier for ADC channel calibration
     * @param channel ADC channel number
     * @param offset Pointer to offset value
     * @param multiplier Pointer to multiplier value
     */
    void configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier);
    /**
     * @brief Check if new data is ready
     * @details Returns true if new ADC data has been processed and is ready to read
     * @return bool True if data is ready
     */
    bool isDataReady();
    
    /**
     * @brief Reset all filters
     * @details Resets all low-pass filters to initial state
     */
    void resetFilters();

private:
    ADC_HandleTypeDef *_pHandler; ///< HAL ADC handler pointer
    size_t bufferSize;            ///< Size of ADC buffer
    
    /**
     * @brief ADC data buffers
     * @details Various buffers for ADC data processing pipeline
     */
    uint32_t *adcDMABuffer;       ///< Double DMA buffer
    uint32_t **adcSamples;        ///< Sample buffer for each channel
    uint32_t *adcAverage;         ///< Averaged values
    float *adcFiltered;           ///< Values after filtering
    uint16_t *adcValue;           ///< Final processed values
    
    /**
     * @brief Calibration parameters
     * @details Offset and multiplier arrays for channel calibration
     */
    uint16_t *offsets;            ///< Offset values for each channel
    uint16_t *multipliers;        ///< Multiplier values for each channel
    
    /**
     * @brief Process control variables
     * @details Variables for controlling data processing
     */
    volatile size_t sampleIndex;  ///< Current sample index
    volatile bool dataReady;      ///< Data ready flag
    
    /**
     * @brief Process DMA data
     * @details Internal function to process data from DMA buffer
     * @param data Pointer to DMA data buffer
     */
    void processDMAData(uint32_t* data);
    
    /**
     * @brief Process ADC data
     * @details Internal function to process and filter ADC data
     */
    void processData();
    
    static uint16_t adcRAW[8];    ///< Static raw ADC values array
};

#endif // __ANALOG_H_
#endif // HAL_ADC_MODULE_ENABLED
