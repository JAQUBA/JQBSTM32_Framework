#include "Analog.h"
#ifdef __ANALOG_H_

Analog *_Analog_instances[ANALOG_MAX_INSTANCES];
uint8_t _Analog_instancesNum = 0;

uint16_t Analog::adcRAW[8];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    Analog::getInstance(hadc)->convCpltCallback();
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    Analog::getInstance(hadc)->convHalfCpltCallback();
}

Analog* Analog::getInstance(ADC_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _Analog_instancesNum; i++) {
        if(_Analog_instances[i]->_pHandler->Instance == pHandler->Instance) 
            return _Analog_instances[i];
    }
    return nullptr;
}

Analog::Analog(ADC_HandleTypeDef *pHandler) : _pHandler(pHandler), bufferSize(pHandler->Init.NbrOfConversion) {
    _Analog_instances[_Analog_instancesNum++] = this;

    // Inicjalizacja buforów
    adcDMABuffer = new uint32_t[bufferSize * ADC_DMA_BUFFER_SIZE];
    adcSamples = new uint32_t*[bufferSize];
    adcAverage = new uint32_t[bufferSize];
    adcFiltered = new float[bufferSize];
    adcValue = new uint16_t[bufferSize];
    
    for(size_t i = 0; i < bufferSize; i++) {
        adcSamples[i] = new uint32_t[ADC_SAMPLES_COUNT];
    }

    // Inicjalizacja kalibracji
    offsets = new uint16_t[bufferSize];
    multipliers = new uint16_t[bufferSize];
    
    for(size_t i = 0; i < bufferSize; i++) {
        offsets[i] = 0;
        multipliers[i] = ADC_DEFAULT_MULTIPLIER;
        adcFiltered[i] = 0.0f;
        adcAverage[i] = 0;
        adcValue[i] = 0;
    }

    // Inicjalizacja zmiennych kontrolnych
    sampleIndex = 0;
    dataReady = false;

    // Kalibracja ADC
    HAL_ADCEx_Calibration_Start(pHandler);

    // Start ADC z DMA
    if (HAL_ADC_Start_DMA(pHandler, adcDMABuffer, bufferSize * ADC_DMA_BUFFER_SIZE) != HAL_OK) {
        Error_Handler();
    }

    // Zadanie przetwarzania danych
    addTaskMain(taskCallback {
        if(dataReady) {
            processData();
            dataReady = false;
        }
        display.toggle(displayMap::S3);
    }, ADC_PROCESS_TASK_PERIOD);
}

void Analog::convCpltCallback() {
    processDMAData(adcDMABuffer + bufferSize);  // Druga połowa bufora
    display.toggle(displayMap::S1);
}

void Analog::convHalfCpltCallback() {
    processDMAData(adcDMABuffer);  // Pierwsza połowa bufora
}

void Analog::processDMAData(uint32_t* data) {
    for(size_t channel = 0; channel < bufferSize; channel++) {
        adcSamples[channel][sampleIndex] = data[channel];
    }
    
    sampleIndex = (sampleIndex + 1) % ADC_SAMPLES_COUNT;
    
    if(sampleIndex == 0) {
        dataReady = true;
    }
}

void Analog::processData() {
    for(size_t channel = 0; channel < bufferSize; channel++) {
        // Uśrednianie próbek
        uint32_t sum = 0;
        for(size_t i = 0; i < ADC_SAMPLES_COUNT; i++) {
            sum += adcSamples[channel][i];
        }
        adcAverage[channel] = sum / ADC_SAMPLES_COUNT;
        
        // Filtracja dolnoprzepustowa
        adcFiltered[channel] = adcFiltered[channel] * (1.0f - ADC_FILTER_ALPHA) + 
                              adcAverage[channel] * ADC_FILTER_ALPHA;
        
        // Kalibracja i konwersja do 16-bit
        int32_t calibrated = ((int32_t)adcFiltered[channel] - offsets[channel]) * multipliers[channel] / 1000;
        
        // Ograniczenie do zakresu 16-bit
        if(calibrated < 0) calibrated = 0;
        if(calibrated > 65535) calibrated = 65535;
        
        adcValue[channel] = (uint16_t)calibrated;
    }
}

uint16_t Analog::getValue(uint8_t channel) {
    if (channel < bufferSize) {
        return adcValue[channel];
    }
    return 0;
}

uint16_t Analog::getRawValue(uint8_t channel) {
    if (channel < bufferSize) {
        return (uint16_t)adcAverage[channel];
    }
    return 0;
}

float Analog::getFilteredValue(uint8_t channel) {
    if (channel < bufferSize) {
        return adcFiltered[channel];
    }
    return 0.0f;
}

void Analog::configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier) {
    if (channel < bufferSize) {
        offsets[channel] = *offset;
        multipliers[channel] = *multiplier;
    }
}

bool Analog::isDataReady() {
    return dataReady;
}

void Analog::resetFilters() {
    for(size_t i = 0; i < bufferSize; i++) {
        adcFiltered[i] = 0.0f;
    }
}

#endif // __ANALOG_H_