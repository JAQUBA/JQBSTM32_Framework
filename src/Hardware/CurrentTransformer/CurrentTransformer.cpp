/*
 * JQBSTM32 Framework - CurrentTransformer.cpp Implementation
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
#include "CurrentTransformer.h"
#include <math.h>
#include <string.h>


void CurrentTransformer::configureChannel(uint16_t *offset, uint16_t *multiplier) {
    _offset = offset;
    _multiplier = multiplier;
}

CurrentTransformer::CurrentTransformer(Analog *analog, uint8_t channel_pos, uint8_t channel_neg, uint16_t vref_mv) :
    _analog(analog),
    _channel_pos(channel_pos),
    _channel_neg(channel_neg),
    _offset(nullptr),
    _multiplier(nullptr),
    _vref_mv(vref_mv),
    _bufferIndex(0),
    _peak_value(0),
    _filtered_rms(0.0f),
    _rms_accumulator(0),
    _rms_counter(0),
    _bufferFull(false),
    _rms_initialized(false) {

    // Wyzeruj bufor jedną operacją (szybsze niż pętla)
    memset(_buffer_samples_peak, 0, sizeof(_buffer_samples_peak));    addTaskMain(taskCallback {
        // Oblicz bezwzględną różnicę między kanałami (zoptymalizowane)
        int32_t diff = (int32_t)_analog->getRawValue(_channel_pos) - 
                       (int32_t)_analog->getRawValue(_channel_neg);
        uint16_t abs_difference = (diff < 0) ? -diff : diff;

        // Dodaj próbkę do bufora
        _buffer_samples_peak[_bufferIndex] = abs_difference;
        
        // Aktualizuj peak value
        if (abs_difference > _peak_value) {
            _peak_value = abs_difference;
        }
        
        // Obliczenia RMS co 16 próbek (32ms) dla stabilności
        if (++_rms_counter >= 16) {
            _rms_counter = 0;
            
            // Oblicz RMS z aktualnego bufora (uproszczony algorytm)
            uint32_t sum_squares = 0;
            uint16_t count = _bufferFull ? CT_SAMPLES_COUNT : (_bufferIndex + 1);
            
            // Szybkie obliczenie sumy kwadratów
            for (uint16_t i = 0; i < count; i++) {
                uint16_t sample = _buffer_samples_peak[i];
                sum_squares += (uint32_t)sample * sample;
            }
            
            // Oblicz RMS i przelicz na napięcie
            float mean_square = (float)sum_squares / count;
            float rms_raw = sqrtf(mean_square);
            float voltage_mv = (rms_raw * _vref_mv) / _analog->getMaxValue();
            
            // Filtr EMA z adaptacyjnym współczynnikiem
            if (!_rms_initialized) {
                _filtered_rms = voltage_mv;
                _rms_initialized = true;
            } else {
                // Silniejszy filtr dla większej stabilności
                _filtered_rms = _filtered_rms * 0.9f + voltage_mv * 0.1f;
            }
        }
        
        // Aktualizuj indeks bufora
        if (++_bufferIndex >= CT_SAMPLES_COUNT) {
            _bufferIndex = 0;
            _bufferFull = true;
            
            // Znajdź nowe maksimum po pełnym cyklu
            _peak_value = 0;
            for (uint16_t i = 0; i < CT_SAMPLES_COUNT; i++) {
                if (_buffer_samples_peak[i] > _peak_value) {
                    _peak_value = _buffer_samples_peak[i];
                }
            }
        }
        
    }, CT_SAMPLING_PERIOD);
}

uint16_t CurrentTransformer::getRawValue() {
    return _peak_value;
}

uint16_t CurrentTransformer::getValue() {
    if (_offset != nullptr && _multiplier != nullptr) {
        // Skalowanie zgodne z implementacją klasy Analog
        uint32_t scaled = ((uint32_t)_peak_value * (*_multiplier)) >> 10;
        if (scaled >= (*_offset)) {
            uint32_t calibrated = scaled - (*_offset);
            return (calibrated > 65535) ? 65535 : (uint16_t)calibrated;
        } else {
            return 0;
        }
    } else {
        return _peak_value;
    }
}

uint16_t CurrentTransformer::getVoltage() {
    // Zwróć przefiltrowaną wartość RMS w mV z zaokrągleniem
    if (_filtered_rms < 0.0f) return 0;
    if (_filtered_rms > 65535.0f) return 65535;
    
    return (uint16_t)(_filtered_rms + 0.5f);
}