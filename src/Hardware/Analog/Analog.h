#include "../../Core.h"
#ifdef HAL_ADC_MODULE_ENABLED

#ifndef __ANALOG_H_
#define __ANALOG_H_

/**
 * @class Analog
 * @brief Klasa do obsługi analogowych wejść.
 */
class Analog {
    public:
        /**
         * @brief Inicjalizuje ADC.
         * @param pHandler Wskaźnik do struktury ADC_HandleTypeDef.
         */
        static void init(ADC_HandleTypeDef *pHandler);

        /**
         * @brief Konstruktor klasy Analog.
         * @param channelNumber Numer kanału ADC.
         */
        Analog(uint8_t channelNumber);

        /**
         * @brief Pobiera surową wartość ADC.
         * @return Surowa wartość ADC.
         */
        uint16_t getRaw();

        /**
         * @brief Pobiera przetworzoną wartość ADC.
         * @return Przetworzona wartość ADC.
         */
        uint16_t getValue();

        /**
         * @brief Konfiguruje kanał ADC.
         * @param offset Wskaźnik do offsetu.
         * @param divider Wskaźnik do dzielnika.
         */
        void configureChannel(uint16_t *offset, uint16_t *divider);

        static uint16_t rawADC[8]; /**< Tablica surowych wartości ADC. */
    private:
        
        uint8_t _channelNumber; /**< Numer kanału ADC. */
        uint16_t *_offset; /**< Wskaźnik do offsetu. */
        uint16_t *_divider; /**< Wskaźnik do dzielnika. */
};

#endif
#endif