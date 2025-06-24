/*
 * JQBSTM32 Framework - CurrentTransformer.h Header
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
#include "../Analog/Analog.h"

#ifndef __CURRENT_TRANSFORMER_H_
#define __CURRENT_TRANSFORMER_H_

#ifndef CT_SAMPLES_COUNT
#define CT_SAMPLES_COUNT 200    ///< Liczba próbek do obliczenia RMS (zwiększone dla lepszej filtracji)
#endif

#ifndef CT_SAMPLING_PERIOD
#define CT_SAMPLING_PERIOD 2    ///< Okres próbkowania w ms (500Hz dla lepszej dokładności)
#endif

#ifndef CT_FILTER_ALPHA
#define CT_FILTER_ALPHA 0.1f    ///< Współczynnik filtracji dolnoprzepustowej (0.3 = szybsza filtracja)
#endif

/**
 * @brief Klasa do obsługi przekładników prądowych AC z pomiarem różnicowym
 * @details Mierzy różnicę napięć między dwoma kanałami ADC i oblicza wartość RMS
 *          dla pomiarów prądu AC przez przekładnik prądowy. Zapewnia:
 *          - Automatyczne próbkowanie różnicy między kanałami
 *          - Obliczanie wartości szczytowej i RMS
 *          - Kalibrację offset/multiplier dla przeliczenia na jednostki prądu
 *          - Filtrację sygnału dla stabilnych pomiarów
 * 
 * @example Przykład użycia:
 * @code
 * Analog* adc = new Analog(&hadc1, 3300);  // 3.3V Vref * CurrentTransformer* ct = new CurrentTransformer(adc, 0, 1, 3300);  // Kanały 0,1 i 3.3V Vref
 * 
 * uint16_t offset = 100;     // Offset kalibracji
 * uint16_t multiplier = 2048; // Mnożnik kalibracji (*1024)
 * ct->configureChannel(&offset, &multiplier);
 *  * // W pętli głównej:
 * uint16_t calibrated_value = ct->getValue();  // Skalibrowana wartość
 * uint16_t voltage_mv = ct->getVoltage();      // Napięcie RMS w mV
 * uint16_t raw_peak = ct->getRawValue();       // Surowa wartość szczytowa
 * @endcode
 */
class CurrentTransformer {
  public:
    /**
     * @brief Konstruktor przekładnika prądowego z pomiarem różnicowym
     * @param analog Wskaźnik do instancji klasy Analog
     * @param channel_pos Kanał ADC dla sygnału dodatniego
     * @param channel_neg Kanał ADC dla sygnału ujemnego
     * @param vref_mv Napięcie referencyjne w milivolach (domyślnie 3300mV)
     * @note Pomiar różnicy między kanałami uruchamia się automatycznie po utworzeniu instancji
     */
    CurrentTransformer(Analog *analog, uint8_t channel_pos, uint8_t channel_neg, uint16_t vref_mv = 3300);
    
    /**
     * @brief Konfiguracja kalibracji przekładnika
     * @param offset Wskaźnik do wartości offset (dynamicznie aktualizowane)
     * @param multiplier Wskaźnik do wartości multiplier (dynamicznie aktualizowane, *1024)
     */
    void configureChannel(uint16_t *offset, uint16_t *multiplier);
    
    /**
     * @brief Pobieranie surowej wartości RMS (przed kalibracją)
     * @return uint16_t Surowa wartość RMS w jednostkach ADC
     */
    uint16_t getRawValue();
    
    /**
     * @brief Pobieranie skalibrowanej wartości szczytowej
     * @return uint16_t Skalibrowana wartość szczytowa po zastosowaniu offset i multiplier
     */
    uint16_t getValue();

    /**
     * @brief Pobieranie napięcia RMS w milivolach
     * @return uint16_t Napięcie RMS w milivolach (mV)
     */
    uint16_t getVoltage();

  private:
    Analog *_analog = nullptr;              ///< Wskaźnik do klasy Analog
    uint8_t _channel_pos = 0;               ///< Kanał ADC dla sygnału dodatniego
    uint8_t _channel_neg = 0;               ///< Kanał ADC dla sygnału ujemnego
    
    // Kalibracja
    uint16_t *_offset = nullptr;            ///< Wskaźnik do wartości offset
    uint16_t *_multiplier = nullptr;        ///< Wskaźnik do wartości multiplier (*1024)
    uint16_t _vref_mv = 3300;               ///< Napięcie referencyjne w milivolach
      uint16_t _buffer_samples_peak[CT_SAMPLES_COUNT]; ///< Bufor różnic między kanałami
    uint16_t _bufferIndex = 0;              ///< Indeks aktualnej próbki
    uint16_t _peak_value = 0;               ///< Aktualna wartość szczytowa z bufora próbek
    float _filtered_rms = 0.0f;             ///< Przefiltrowana wartość RMS
    uint32_t _rms_accumulator = 0;          ///< Akumulator dla RMS (32-bit wystarczy)
    uint8_t _rms_counter = 0;               ///< Licznik dla obliczania RMS co N próbek
    bool _bufferFull : 1;                   ///< Flaga pełnego bufora (1 bit)
    bool _rms_initialized : 1;              ///< Flaga inicjalizacji RMS (1 bit)
};

#endif // __CURRENT_TRANSFORMER_H_
