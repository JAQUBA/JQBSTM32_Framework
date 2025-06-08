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
#define ANALOG_MAX_INSTANCES 1
#endif

// Definitions for filtering and averaging
#ifndef ADC_SAMPLES_COUNT
#define ADC_SAMPLES_COUNT 64    // Number of samples for averaging
#endif

#ifndef ADC_FILTER_ALPHA
#define ADC_FILTER_ALPHA 0.1f   // Low-pass filter coefficient (0-1)
#endif

#ifndef ADC_DMA_BUFFER_SIZE
#define ADC_DMA_BUFFER_SIZE 2   // DMA buffer size (double buffer)
#endif

#ifndef ADC_DEFAULT_MULTIPLIER
#define ADC_DEFAULT_MULTIPLIER 1000  // Default multiplier (1.0 in fixed point)
#endif

#ifndef ADC_PROCESS_TASK_PERIOD
#define ADC_PROCESS_TASK_PERIOD 10   // Data processing task period [ms]
#endif

class Analog {
public:
    static Analog* getInstance(ADC_HandleTypeDef *pHandler);
    Analog(ADC_HandleTypeDef *pHandler);
    
    void convCpltCallback();
    void convHalfCpltCallback();
    
    uint16_t getValue(uint8_t channel);
    uint16_t getRawValue(uint8_t channel);
    float getFilteredValue(uint8_t channel);
    
    void configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier);
    bool isDataReady();
    void resetFilters();

private:
    ADC_HandleTypeDef *_pHandler;
    size_t bufferSize;
    
    // DMA and processing buffers
    uint32_t *adcDMABuffer;        // Double DMA buffer
    uint32_t **adcSamples;         // Sample buffer for each channel
    uint32_t *adcAverage;          // Averaged values
    float *adcFiltered;            // Values after filtering
    uint16_t *adcValue;            // Final values
    
    // Calibration
    uint16_t *offsets;
    uint16_t *multipliers;
    
    // Process control
    volatile size_t sampleIndex;
    volatile bool dataReady;
    
    void processDMAData(uint32_t* data);
    void processData();
    
    static uint16_t adcRAW[8];
};

#endif // __ANALOG_H_
#endif // HAL_ADC_MODULE_ENABLED
