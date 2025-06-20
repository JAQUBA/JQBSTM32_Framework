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

// Statyczna lista instancji dla callback
static CurrentTransformer* _ct_instances[2] = {nullptr, nullptr}; // Tylko 2 instancje (pary kanałów)
static uint8_t _ct_instance_count = 0;

CurrentTransformer::CurrentTransformer(Analog *analog, uint8_t channel_pos, uint8_t channel_neg, uint16_t ratio, uint16_t burden_ohm) :
    _analog(analog),
    _channel_pos(channel_pos),
    _channel_neg(channel_neg),
    _ratio(ratio),
    _burden_ohm(burden_ohm),
    _sampleIndex(0),
    _bufferFull(false),
    _isRunning(false),
    _calibration_offset(nullptr),
    _calibration_multiplier(nullptr) {    // Oblicz połowę Vref w jednostkach ADC (10-bit = 0-1023, więc połowa to 511.5 ≈ 512)
    _vref_half = 512;
    
    // Inicjalizacja filtracji
    _filtered_rms = 0.0f;
    _measurement_count = 0;
    for (uint8_t i = 0; i < 8; i++) {
        _rms_buffer[i] = 0.0f;
    }
    
    // Wyczyść bufory próbek
    for (uint16_t i = 0; i < CT_SAMPLES_COUNT; i++) {
        _samples_pos[i] = _vref_half;
        _samples_neg[i] = _vref_half;
    }
      // Dodaj do listy instancji
    if (_ct_instance_count < 2) {
        _ct_instances[_ct_instance_count++] = this;
    }
    
    // Automatyczne uruchomienie pomiaru
    start();
}

void CurrentTransformer::start() {
    _isRunning = true;
    _sampleIndex = 0;
    _bufferFull = false;
    
    // Uruchom task do próbkowania (tylko raz dla wszystkich instancji)
    static bool task_started = false;
    if (!task_started) {
        task_started = true;
        addTaskMain(taskCallback {
            // Próbkuj wszystkie aktywne instancje
            for (uint8_t i = 0; i < _ct_instance_count; i++) {
                if (_ct_instances[i] && _ct_instances[i]->_isRunning) {
                    _ct_instances[i]->sample();
                }
            }
        }, CT_SAMPLING_PERIOD);
    }
}

void CurrentTransformer::stop() {
    _isRunning = false;
}

void CurrentTransformer::sample() {
    if (!_isRunning) return;
    
    // Pobierz surowe wartości ADC z obu kanałów
    uint16_t raw_pos = _analog->getRawValue(_channel_pos);
    uint16_t raw_neg = _analog->getRawValue(_channel_neg);
    
    // Zapisz próbki w buforach
    _samples_pos[_sampleIndex] = raw_pos;
    _samples_neg[_sampleIndex] = raw_neg;
    
    // Przesuń indeks
    _sampleIndex++;
    if (_sampleIndex >= CT_SAMPLES_COUNT) {
        _sampleIndex = 0;
        _bufferFull = true;
    }
}

float CurrentTransformer::calculateRMS() {
    if (!_bufferFull && _sampleIndex < CT_SAMPLES_COUNT / 4) {
        return _filtered_rms; // Zwróć ostatnią przefiltrowaną wartość jeśli za mało próbek
    }
    
    uint32_t sum_squares = 0;
    uint16_t sample_count = _bufferFull ? CT_SAMPLES_COUNT : _sampleIndex;
    uint16_t valid_samples = 0;
    
    // Oblicz sumę kwadratów odchyleń od Vref/2 dla obu kanałów z dodatkową filtracją
    for (uint16_t i = 0; i < sample_count; i++) {
        // Kanał dodatni - wartość powyżej Vref/2 z progiem szumów
        int32_t deviation_pos = 0;
        if (_samples_pos[i] > (_vref_half + 5)) { // Próg 5 ADC jednostek dla eliminacji szumów
            deviation_pos = (int32_t)_samples_pos[i] - (int32_t)_vref_half;
            valid_samples++;
        }
        
        // Kanał ujemny - wartość powyżej Vref/2 z progiem szumów
        int32_t deviation_neg = 0;
        if (_samples_neg[i] > (_vref_half + 5)) { // Próg 5 ADC jednostek dla eliminacji szumów
            deviation_neg = (int32_t)_samples_neg[i] - (int32_t)_vref_half;
            valid_samples++;
        }
        
        // Suma kwadratów z obu kanałów (rekonstrukcja pełnego sygnału AC)
        sum_squares += (uint32_t)(deviation_pos * deviation_pos);
        sum_squares += (uint32_t)(deviation_neg * deviation_neg);
    }
    
    // Sprawdź czy mamy wystarczająco dużo ważnych próbek
    if (valid_samples < (sample_count / 4)) {
        return _filtered_rms; // Za mało ważnych próbek, zwróć ostatnią wartość
    }
      // Oblicz RMS w jednostkach ADC
    float rms_adc = sqrtf((float)sum_squares / (2.0f * sample_count));
    
    // Przelicz na napięcie RMS w mV (10-bit ADC: 0-1023)
    float rms_voltage_mv = (rms_adc * CT_VREF_MV) / 1023.0f;
    
    // Dodaj do bufora uśredniającego
    _rms_buffer[_measurement_count % 8] = rms_voltage_mv;
    _measurement_count++;
    
    // Oblicz średnią ruchomą z ostatnich 8 pomiarów
    float average_rms = 0.0f;
    uint8_t count = (_measurement_count < 8) ? _measurement_count : 8;
    for (uint8_t i = 0; i < count; i++) {
        average_rms += _rms_buffer[i];
    }
    average_rms /= count;
    
    // Filtr dolnoprzepustowy (EMA - Exponential Moving Average)
    if (_measurement_count == 1) {
        _filtered_rms = average_rms; // Pierwszy pomiar
    } else {
        _filtered_rms = (CT_FILTER_ALPHA * average_rms) + ((1.0f - CT_FILTER_ALPHA) * _filtered_rms);
    }
    
    return _filtered_rms;
}

float CurrentTransformer::getCurrentRMS() {
    float voltage_rms_mv = calculateRMS();
    
    if (voltage_rms_mv <= 0.0f) return 0.0f;
    
    // Przelicz napięcie na prąd przez przekładnik
    // I_primary = (U_rms / R_burden) * przekładnia
    float current_secondary_ma = voltage_rms_mv / _burden_ohm; // mA
    float current_primary_a = (current_secondary_ma / 1000.0f) * _ratio; // A
    
    return current_primary_a;
}

uint16_t CurrentTransformer::getRawValuePos() {
    return _analog->getRawValue(_channel_pos);
}

uint16_t CurrentTransformer::getRawValueNeg() {
    return _analog->getRawValue(_channel_neg);
}

float CurrentTransformer::getVoltageRMS() {
    return calculateRMS() / 1000.0f; // Konwersja mV na V
}

bool CurrentTransformer::isReady() {
    return _bufferFull || (_sampleIndex >= CT_SAMPLES_COUNT / 2);
}

void CurrentTransformer::reset() {
    _sampleIndex = 0;
    _bufferFull = false;
    
    // Wyczyść bufory próbek
    for (uint16_t i = 0; i < CT_SAMPLES_COUNT; i++) {
        _samples_pos[i] = _vref_half;
        _samples_neg[i] = _vref_half;
    }
    
    // Wyczyść filtry
    _filtered_rms = 0.0f;
    _measurement_count = 0;
    for (uint8_t i = 0; i < 8; i++) {
        _rms_buffer[i] = 0.0f;
    }
}

void CurrentTransformer::setCalibration(uint16_t *offset, uint16_t *multiplier) {
    _calibration_offset = offset;
    _calibration_multiplier = multiplier;
}

float CurrentTransformer::getCalibratedCurrentRMS() {
    float raw_current = getCurrentRMS();
    
    // Sprawdź czy kalibracja jest skonfigurowana
    if (_calibration_offset == nullptr || _calibration_multiplier == nullptr) {
        return raw_current; // Zwróć surową wartość jeśli brak kalibracji
    }
      // Konwertuj wartości uint16_t na float (skalowanie /16384 przeskalowane do /1000 dla czytelności)
    float offset_float = (float)(*_calibration_offset) / 16384.0f;
    float multiplier_float = (float)(*_calibration_multiplier) / 16384.0f;
    
    // Zastosuj kalibrację: output = (input + offset) * multiplier
    float calibrated_current = (raw_current + offset_float) * multiplier_float;
    
    // Upewnij się, że wartość nie jest ujemna
    if (calibrated_current < 0.0f) {
        calibrated_current = 0.0f;
    }
    
    return calibrated_current;
}

uint16_t CurrentTransformer::getCalibratedValue1000() {
    float calibrated_current = getCalibratedCurrentRMS();
    
    // Przeskaluj do zakresu 0-1000
    // Zakładamy maksymalny prąd np. 100A odpowiada wartości 1000
    // Można to dostosować według potrzeb projektu
    uint32_t scaled_value = (uint32_t)(calibrated_current * 10.0f); // 1A = 10 jednostek
    
    // Ogranicz do zakresu 0-1000
    if (scaled_value > 1000) {
        scaled_value = 1000;
    }
    
    return (uint16_t)scaled_value;
}

float CurrentTransformer::getRawRMS() {
    if (!_bufferFull && _sampleIndex < CT_SAMPLES_COUNT / 4) {
        return 0.0f; // Za mało próbek
    }
    
    uint32_t sum_squares = 0;
    uint16_t sample_count = _bufferFull ? CT_SAMPLES_COUNT : _sampleIndex;
    
    // Oblicz surową sumę kwadratów bez filtracji szumów
    for (uint16_t i = 0; i < sample_count; i++) {
        // Kanał dodatni - wartość powyżej Vref/2
        int32_t deviation_pos = 0;
        if (_samples_pos[i] > _vref_half) {
            deviation_pos = (int32_t)_samples_pos[i] - (int32_t)_vref_half;
        }
        
        // Kanał ujemny - wartość powyżej Vref/2
        int32_t deviation_neg = 0;
        if (_samples_neg[i] > _vref_half) {
            deviation_neg = (int32_t)_samples_neg[i] - (int32_t)_vref_half;
        }
        
        // Suma kwadratów z obu kanałów
        sum_squares += (uint32_t)(deviation_pos * deviation_pos);
        sum_squares += (uint32_t)(deviation_neg * deviation_neg);
    }
      // Oblicz RMS w jednostkach ADC
    float rms_adc = sqrtf((float)sum_squares / (2.0f * sample_count));
    
    // Przelicz na napięcie RMS w mV (10-bit ADC: 0-1023)
    float rms_voltage_mv = (rms_adc * CT_VREF_MV) / 1023.0f;
    
    return rms_voltage_mv;
}
