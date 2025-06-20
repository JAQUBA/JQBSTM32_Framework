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
#define CT_FILTER_ALPHA 0.1f    ///< Współczynnik filtracji dolnoprzepustowej (0.1 = mocna filtracja)
#endif

#ifndef CT_VREF_MV
#define CT_VREF_MV 3300         ///< Napięcie referencyjne w mV
#endif

/**
 * @brief Klasa do obsługi przekładników prądowych AC z prostowaniem dwupołówkowym
 * @details Oblicza wartość RMS prądu z dwóch kanałów: dodatniej i ujemnej połówki
 */
class CurrentTransformer {
public:    /**
     * @brief Konstruktor przekładnika prądowego z prostowaniem dwupołówkowym
     * @param analog Wskaźnik do instancji klasy Analog
     * @param channel_pos Kanał ADC dla dodatniej połówki sygnału
     * @param channel_neg Kanał ADC dla ujemnej połówki sygnału
     * @param ratio Przekładnia przekładnika (np. 1000 dla 1000:1)
     * @param burden_ohm Rezystancja obciążenia w ohm (np. 33 dla 33Ω)
     * @note Pomiar uruchamia się automatycznie po utworzeniu instancji
     */
    CurrentTransformer(Analog *analog, uint8_t channel_pos, uint8_t channel_neg, uint16_t ratio, uint16_t burden_ohm);
      /**
     * @brief Uruchomienie pomiaru prądu
     * @details Rozpoczyna cykliczne próbkowanie sygnału AC z obu kanałów
     * @note Wywołane automatycznie w konstruktorze, można używać do ponownego uruchomienia
     */
    void start();
    
    /**
     * @brief Zatrzymanie pomiaru prądu
     */
    void stop();
    
    /**
     * @brief Pobieranie próbki sygnału AC z obu kanałów
     * @details Funkcja wywoływana cyklicznie do zbierania próbek
     */
    void sample();
    
    /**
     * @brief Obliczenie wartości RMS prądu
     * @return float Wartość RMS prądu w amperach
     */
    float getCurrentRMS();
    
    /**
     * @brief Pobieranie surowej wartości ADC kanału dodatniego
     * @return uint16_t Aktualna wartość ADC kanału dodatniego
     */
    uint16_t getRawValuePos();
    
    /**
     * @brief Pobieranie surowej wartości ADC kanału ujemnego
     * @return uint16_t Aktualna wartość ADC kanału ujemnego
     */
    uint16_t getRawValueNeg();
    
    /**
     * @brief Pobieranie napięcia AC na przekładniku
     * @return float Napięcie RMS w wolt
     */
    float getVoltageRMS();
    
    /**
     * @brief Sprawdzenie czy pomiar jest gotowy
     * @return bool True jeśli zebrano wystarczająco próbek
     */
    bool isReady();
      /**
     * @brief Reset bufora próbek
     */
    void reset();    /**
     * @brief Konfiguracja kalibracji przekładnika
     * @param offset Wskaźnik do wartości offsetu (w jednostkach *1000, dynamicznie aktualizowana)
     * @param multiplier Wskaźnik do mnożnika kalibracyjnego (w jednostkach *1000, dynamicznie aktualizowany)
     */
    void setCalibration(uint16_t *offset, uint16_t *multiplier);
    
    /**
     * @brief Pobieranie skalibrowanej wartości RMS prądu
     * @return float Skalibrowana wartość RMS prądu w amperach
     */
    float getCalibratedCurrentRMS();
      /**
     * @brief Pobieranie skalibrowanej wartości w zakresie 0-1000
     * @return uint16_t Wartość 0-1000 odpowiadająca prądowi (po kalibracji)
     */
    uint16_t getCalibratedValue1000();
    
    /**
     * @brief Pobieranie surowej (nieprzefiltrowanej) wartości RMS
     * @return float Surowa wartość RMS bez filtracji
     */
    float getRawRMS();

private:
    Analog *_analog;              ///< Wskaźnik do klasy Analog
    uint8_t _channel_pos;         ///< Kanał ADC dla dodatniej połówki
    uint8_t _channel_neg;         ///< Kanał ADC dla ujemnej połówki
    uint16_t _ratio;              ///< Przekładnia przekładnika
    uint16_t _burden_ohm;         ///< Rezystancja obciążenia w ohm
    
    uint16_t _samples_pos[CT_SAMPLES_COUNT]; ///< Bufor próbek ADC kanału dodatniego
    uint16_t _samples_neg[CT_SAMPLES_COUNT]; ///< Bufor próbek ADC kanału ujemnego
    uint16_t _sampleIndex;        ///< Indeks aktualnej próbki
    bool _bufferFull;             ///< Flaga pełnego bufora
    bool _isRunning;              ///< Flaga działania
      uint16_t _vref_half;          ///< Połowa napięcia referencyjnego w jednostkach ADC
    
    // Parametry kalibracji
    uint16_t *_calibration_offset;    ///< Wskaźnik do offsetu kalibracyjnego (*1000)
    uint16_t *_calibration_multiplier; ///< Wskaźnik do mnożnika kalibracyjnego (*1000)
    
    // Filtracja i uśrednianie
    float _filtered_rms;          ///< Przefiltrowana wartość RMS
    uint8_t _measurement_count;   ///< Licznik pomiarów dla uśredniania
    float _rms_buffer[8];         ///< Bufor ostatnich pomiarów RMS do uśredniania
    
    /**
     * @brief Obliczenie wartości RMS z buforów próbek obu kanałów
     * @return float Wartość RMS napięcia w mV
     */
    float calculateRMS();
};

#endif // __CURRENT_TRANSFORMER_H_
