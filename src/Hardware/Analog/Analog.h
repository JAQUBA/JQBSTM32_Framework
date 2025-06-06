#include "../../Core.h"
#ifdef HAL_ADC_MODULE_ENABLED

#ifndef __ANALOG_H_
#define __ANALOG_H_

#ifndef ANALOG_MAX_INSTANCES
#define ANALOG_MAX_INSTANCES 1
#endif

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


#include "Application/Display/Display.h"
extern Display display;

class Analog {

private:
    ADC_HandleTypeDef *_pHandler;
    size_t bufferSize;
    
    // Bufory DMA i przetwarzania
    uint32_t *adcDMABuffer;        // Podwójny bufor DMA
    uint32_t **adcSamples;         // Bufor próbek dla każdego kanału
    uint32_t *adcAverage;          // Uśrednione wartości
    float *adcFiltered;            // Wartości po filtracji
    uint16_t *adcValue;            // Końcowe wartości
    
    // Kalibracja
    uint16_t *offsets;
    uint16_t *multipliers;
    
    // Kontrola przetwarzania
    volatile size_t sampleIndex;
    volatile bool dataReady;
    
    void processDMAData(uint32_t* data);
    void processData();

public:
    static Analog* getInstance(ADC_HandleTypeDef *pHandler);
    Analog(ADC_HandleTypeDef *pHandler);
    
    void convCpltCallback();
    void convHalfCpltCallback();
    
    uint16_t getValue(uint8_t channel);
    uint16_t getRawValue(uint8_t channel);
    float getFilteredValue(uint8_t channel);
    
    void configureChannel(uint8_t channel, uint16_t offset, uint16_t multiplier);
    bool isDataReady();
    void resetFilters();

    public:
        static Analog* getInstance(ADC_HandleTypeDef *pHandler);
        Analog(ADC_HandleTypeDef *pHandler);
        void configureChannel(uint8_t channel, uint16_t *offset, uint16_t *multiplier);

        uint16_t getValue(uint8_t channel);
        
        void convCpltCallback();
    private:
        ADC_HandleTypeDef *_pHandler;
        uint32_t bufferSize;
        static uint16_t adcRAW[8];

        uint32_t *adcBuffer;
        uint16_t adcSamplesNum;

        uint32_t *adcValue;

        uint16_t *offsets;
        uint16_t *multipliers;
};

#endif
#endif