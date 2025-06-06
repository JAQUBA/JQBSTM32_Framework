#include "Analog.h"
#ifdef __ANALOG_H_


Analog *_Analog_instances[ANALOG_MAX_INSTANCES];
uint8_t _Analog_instancesNum = 0;

uint16_t Analog::adcRAW[8];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {Analog::getInstance(hadc)->convCpltCallback();}


// Definicje dla filtrowania i uśredniania
#ifndef ADC_SAMPLES_COUNT
#define ADC_SAMPLES_COUNT 64    // Liczba próbek do uśredniania
#endif

#ifndef ADC_FILTER_ALPHA
#define ADC_FILTER_ALPHA 0.1f   // Współczynnik filtra dolnoprzepustowego (0-1)
#endif

#ifndef ADC_DMA_BUFFER_SIZE
#define ADC_DMA_BUFFER_SIZE 2   // Rozmiar bufora DMA (podwójny bufor)
#endif

#ifndef ADC_DEFAULT_MULTIPLIER
#define ADC_DEFAULT_MULTIPLIER 1000  // Domyślny mnożnik (1.0 w fixed point)
#endif

#ifndef ADC_PROCESS_TASK_PERIOD
#define ADC_PROCESS_TASK_PERIOD 10   // Okres zadania przetwarzania danych [ms]
#endif

Analog *_Analog_instances[ANALOG_MAX_INSTANCES];
uint8_t _Analog_instancesNum = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    Analog::getInstance(hadc)->convCpltCallback();
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    Analog::getInstance(hadc)->convHalfCpltCallback();
}

Analog* Analog::getInstance(ADC_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _Analog_instancesNum; i++) {
        if(_Analog_instances[i]->_pHandler->Instance == pHandler->Instance) return _Analog_instances[i];
    }
    return nullptr;

}

Analog::Analog(ADC_HandleTypeDef *pHandler) : _pHandler(pHandler), bufferSize(pHandler->Init.NbrOfConversion) {
    _Analog_instances[_Analog_instancesNum++] = this;

    

    // adcRAW = new uint32_t[bufferSize];

    adcBuffer = new uint32_t[bufferSize];

    adcValue = new uint32_t[bufferSize];

    offsets = new uint16_t[bufferSize];
    multipliers = new uint16_t[bufferSize];

    HAL_ADCEx_Calibration_Start(pHandler);

    if (HAL_ADC_Start_DMA(pHandler, adcBuffer, bufferSize) != HAL_OK) {
        Error_Handler();
    }
    addTaskMain(taskCallback {
        for (size_t i = 0; i < bufferSize; i++) {
            // adcValue[i] = ((adcBuffer[i] / adcSamplesNum));
            // adcBuffer[i] = 0;
        }
        // adcSamplesNum = 0;
        display.toggle(displayMap::S3);//, true);
    }, 1000);
}
void Analog::convCpltCallback() {
    display.toggle(displayMap::S1);//, true);
    for (size_t i = 0; i < bufferSize; i++) {
        // adcBuffer[i] = adcRAW[i];
        // adcSamplesNum++;
    }
}


}

Analog::Analog(ADC_HandleTypeDef *pHandler) : 
    _pHandler(pHandler), 
    bufferSize(pHandler->Init.NbrOfConversion),
    sampleIndex(0),
    dataReady(false) {
    
    _Analog_instances[_Analog_instancesNum++] = this;

    // Alokuj bufory DMA (podwójny bufor dla ciągłej konwersji)
    adcDMABuffer = new uint32_t[bufferSize * ADC_DMA_BUFFER_SIZE];
    
    // Bufory dla próbek i uśredniania
    adcSamples = new uint32_t*[bufferSize];
    for (size_t i = 0; i < bufferSize; i++) {
        adcSamples[i] = new uint32_t[ADC_SAMPLES_COUNT];
        memset(adcSamples[i], 0, ADC_SAMPLES_COUNT * sizeof(uint32_t));
    }
    
    // Bufory dla wyników
    adcAverage = new uint32_t[bufferSize];
    adcFiltered = new float[bufferSize];
    adcValue = new uint16_t[bufferSize];
    
    // Konfiguracja kalibracji
    offsets = new uint16_t[bufferSize];
    multipliers = new uint16_t[bufferSize];
    
    // Inicjalizacja wartości domyślnych
    for (size_t i = 0; i < bufferSize; i++) {
        adcAverage[i] = 0;
        adcFiltered[i] = 0.0f;
        adcValue[i] = 0;
        offsets[i] = 0;
        multipliers[i] = ADC_DEFAULT_MULTIPLIER;
    }

    // Kalibracja ADC
    HAL_ADCEx_Calibration_Start(pHandler);

    // Start DMA w trybie cyklicznym
    if (HAL_ADC_Start_DMA(pHandler, adcDMABuffer, bufferSize * ADC_DMA_BUFFER_SIZE) != HAL_OK) {
        Error_Handler();
    }
    
    // Zadanie przetwarzania danych
    addTaskMain(taskCallback {
        if (dataReady) {
            processData();
            dataReady = false;
            display.toggle(displayMap::S3);
        }
    }, ADC_PROCESS_TASK_PERIOD);
}
void Analog::convCpltCallback() {
    // Przetwórz drugą połowę bufora DMA
    processDMAData(&adcDMABuffer[bufferSize]);
    display.toggle(displayMap::S1);
}

void Analog::convHalfCpltCallback() {
    // Przetwórz pierwszą połowę bufora DMA
    processDMAData(&adcDMABuffer[0]);
    display.toggle(displayMap::S2);
}

void Analog::processDMAData(uint32_t* data) {
    // Skopiuj nowe dane do bufora próbek
    for (size_t channel = 0; channel < bufferSize; channel++) {
        adcSamples[channel][sampleIndex] = data[channel];
    }
    
    // Zwiększ indeks próbki
    sampleIndex = (sampleIndex + 1) % ADC_SAMPLES_COUNT;
    
    // Oznacz że dane są gotowe do przetworzenia
    dataReady = true;
}

void Analog::processData() {
    for (size_t channel = 0; channel < bufferSize; channel++) {
        // Oblicz średnią z próbek
        uint64_t sum = 0;
        for (size_t i = 0; i < ADC_SAMPLES_COUNT; i++) {
            sum += adcSamples[channel][i];
        }
        adcAverage[channel] = sum / ADC_SAMPLES_COUNT;
        
        // Zastosuj filtr dolnoprzepustowy
        adcFiltered[channel] = adcFiltered[channel] * (1.0f - ADC_FILTER_ALPHA) + 
                              adcAverage[channel] * ADC_FILTER_ALPHA;
        
        // Zastosuj kalibrację i konwersję do końcowej wartości
        uint32_t calibrated = ((uint32_t)adcFiltered[channel] + offsets[channel]) * multipliers[channel] / 1000;
        
        // Ogranicz do 16-bit
        adcValue[channel] = (calibrated > 65535) ? 65535 : (uint16_t)calibrated;
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

void Analog::configureChannel(uint8_t channel, uint16_t offset, uint16_t multiplier) {
    if (channel < bufferSize) {
        offsets[channel] = offset;
        multipliers[channel] = multiplier;
    }
}

bool Analog::isDataReady() {
    return !dataReady; // Zwraca true gdy dane nie są przetwarzane
}

void Analog::resetFilters() {
    for (size_t i = 0; i < bufferSize; i++) {
        adcFiltered[i] = adcAverage[i];

Analog *_Analog_instances[ANALOG_MAX_INSTANCES];
uint8_t _Analog_instancesNum = 0;

uint16_t Analog::adcRAW[8];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {Analog::getInstance(hadc)->convCpltCallback();}

Analog* Analog::getInstance(ADC_HandleTypeDef *pHandler) {
    for (size_t i = 0; i < _Analog_instancesNum; i++) {
        if(_Analog_instances[i]->_pHandler->Instance == pHandler->Instance) return _Analog_instances[i];
    }
    return nullptr;
}

Analog::Analog(ADC_HandleTypeDef *pHandler) : _pHandler(pHandler), bufferSize(pHandler->Init.NbrOfConversion) {
    _Analog_instances[_Analog_instancesNum++] = this;

    

    // adcRAW = new uint32_t[bufferSize];

    adcBuffer = new uint32_t[bufferSize];

    adcValue = new uint32_t[bufferSize];

    offsets = new uint16_t[bufferSize];
    multipliers = new uint16_t[bufferSize];

    HAL_ADCEx_Calibration_Start(pHandler);

    if (HAL_ADC_Start_DMA(pHandler, adcBuffer, bufferSize) != HAL_OK) {
        Error_Handler();
    }
    addTaskMain(taskCallback {
        for (size_t i = 0; i < bufferSize; i++) {
            // adcValue[i] = ((adcBuffer[i] / adcSamplesNum));
            // adcBuffer[i] = 0;
        }
        // adcSamplesNum = 0;
        display.toggle(displayMap::S3);//, true);
    }, 1000);
}
void Analog::convCpltCallback() {
    display.toggle(displayMap::S1);//, true);
    for (size_t i = 0; i < bufferSize; i++) {
        // adcBuffer[i] = adcRAW[i];
        // adcSamplesNum++;
    }
}


uint16_t Analog::getValue(uint8_t channel) {
    // if (channel < bufferSize) {
        return 200;
    // }
    // return 0;
}
void Analog::configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier) {
    if (channel < bufferSize) {
        offsets[channel] = *offset;
        multipliers[channel] = *multiplier;
    }
}
#endif