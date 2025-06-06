/**
 * @file DS18B20_Example.cpp
 * @brief Przykład użycia biblioteki DS18B20Manager
 * 
 * Ten przykład pokazuje różne sposoby konfiguracji i użycia czujników DS18B20:
 * 1. Dodawanie czujników z kodem ROM
 * 2. Odczytywanie ROM z magistrali
 * 3. Automatyczny odczyt temperatury
 * 4. Ręczny odczyt temperatury
 */

#include "Core.h"
#include "Hardware/OneWire/OneWire.h"
#include "Device/DS18B20/DS18B20.h"
#include "Hardware/FlashMemory/FlashMemory.h"
#include "Hardware/Timer/Timer.h"
#include "tim.h"

// Inicjalizacja sprzętu
OneWire ow(new Timer(&htim7), PIN_OW);
FlashMemory flashMemory;
DS18B20Manager ds18b20Manager(&ow, &flashMemory);

// Identyfikatory czujników
#define SENSOR_INTERNAL     0
#define SENSOR_EXTERNAL     1
#define SENSOR_AMBIENT      2

void setup() {
    // === PRZYKŁAD 1: Dodawanie czujników z znanymi kodami ROM ===
    
    // Sposób 1: Bezpośrednio z wartością 64-bit
    uint64_t internalROM = 0x28FF123456789ABC;  // Przykładowy kod ROM
    ds18b20Manager.addSensor(SENSOR_INTERNAL, internalROM);
    
    // Sposób 2: Z pamięci flash
    uint64_t externalROM = flashMemory.read(0x08013000);
    ds18b20Manager.addSensor(SENSOR_EXTERNAL, externalROM);
    
    // Sposób 3: Używając makra (jak w oryginalnym kodzie)
    #define AMBIENT_ROM flashMemory.read(0x08013800)
    ds18b20Manager.addSensor(SENSOR_AMBIENT, AMBIENT_ROM);
    
    // === PRZYKŁAD 2: Odczytywanie ROM z magistrali ===
    /*
    // Użyj tego gdy chcesz odkryć kod ROM nowego czujnika
    ds18b20Manager.readRomFromBus([](uint64_t romCode) {
        char buffer[64];
        sprintf(buffer, "Znaleziony czujnik ROM: 0x%016llX\r\n", romCode);
        // Wyślij przez UART lub zapisz do pamięci flash
        
        // Opcjonalnie dodaj automatycznie
        ds18b20Manager.addSensor(3, romCode);
    });
    */
    
    // === PRZYKŁAD 3: Automatyczny odczyt temperatury ===
    
    // Włącz automatyczny odczyt co 2 sekundy
    ds18b20Manager.enableAutoRead(2000);
    
    // Ustaw callback dla automatycznego odczytu
    ds18b20Manager.setAutoReadCallback([](uint8_t sensorId, float temperature, bool success) {
        if(success) {
            const char* sensorNames[] = {"Internal", "External", "Ambient"};
            char buffer[64];
            sprintf(buffer, "Auto: %s = %.2f°C\r\n", 
                   sensorNames[sensorId], temperature);
            // rs485.transmit((uint8_t*)buffer, strlen(buffer));
        } else {
            char buffer[64];
            sprintf(buffer, "Error reading sensor %d\r\n", sensorId);
            // rs485.transmit((uint8_t*)buffer, strlen(buffer));
        }
    });
    
    // === PRZYKŁAD 4: Ręczny odczyt pojedynczego czujnika ===
    
    addTaskMain(taskCallback {
        ds18b20Manager.readTemperature(SENSOR_INTERNAL, [](uint8_t id, float temp, bool success) {
            if(success) {
                char buffer[32];
                sprintf(buffer, "Manual: Internal = %.2f°C\r\n", temp);
                // rs485.transmit((uint8_t*)buffer, strlen(buffer));
            }
        });
    }, 5000); // Co 5 sekund
    
    // === PRZYKŁAD 5: Odczyt wszystkich czujników jednocześnie ===
    
    addTaskMain(taskCallback {
        ds18b20Manager.readAllTemperatures([](uint8_t id, float temp, bool success) {
            if(success) {
                char buffer[32];
                sprintf(buffer, "Batch[%d]: %.2f°C\r\n", id, temp);
                // rs485.transmit((uint8_t*)buffer, strlen(buffer));
            }
        });
    }, 10000); // Co 10 sekund
    
    // === PRZYKŁAD 6: Pobieranie ostatnich wartości ===
    
    addTaskMain(taskCallback {
        for(uint8_t i = 0; i < ds18b20Manager.getSensorCount(); i++) {
            if(ds18b20Manager.isSensorValid(i)) {
                float temp = ds18b20Manager.getTemperature(i);
                int16_t rawTemp = ds18b20Manager.getRawTemperature(i);
                uint64_t rom = ds18b20Manager.getSensorRom(i);
                
                char buffer[64];
                sprintf(buffer, "Sensor %d: %.2f°C (raw:%d, ROM:0x%016llX)\r\n", 
                       i, temp, rawTemp, rom);
                // rs485.transmit((uint8_t*)buffer, strlen(buffer));
            }
        }
    }, 15000); // Co 15 sekund
    
    // === PRZYKŁAD 7: Kontrola automatycznego odczytu ===
    
    addTaskMain(taskCallback {
        static bool autoReadEnabled = true;
        autoReadEnabled = !autoReadEnabled;
        
        if(autoReadEnabled) {
            ds18b20Manager.enableAutoRead(1000);
            // rs485.transmit((uint8_t*)"Auto-read ON\r\n", 14);
        } else {
            ds18b20Manager.disableAutoRead();
            // rs485.transmit((uint8_t*)"Auto-read OFF\r\n", 15);
        }
    }, 30000); // Przełączaj co 30 sekund
}

void loop() {
    // Główna pętla - DS18B20Manager działa automatycznie w tle
}

// === PRZYKŁADY UŻYCIA W APLIKACJI ===

// Funkcja do sprawdzania temperatury z warunkami
bool checkTemperatureAlarms() {
    float internalTemp = ds18b20Manager.getTemperature(SENSOR_INTERNAL);
    float externalTemp = ds18b20Manager.getTemperature(SENSOR_EXTERNAL);
    
    // Sprawdź czy temperatura wewnętrzna nie jest za wysoka
    if(internalTemp > 85.0f) {
        // Alarm - za wysoka temperatura wewnętrzna
        return false;
    }
    
    // Sprawdź różnicę temperatur
    if(abs(internalTemp - externalTemp) > 20.0f) {
        // Alarm - za duża różnica temperatur
        return false;
    }
    
    return true; // Wszystko OK
}

// Funkcja do logowania temperatury do pamięci
void logTemperature() {
    static uint32_t logAddress = 0x08014000; // Adres w flash dla logów
    
    for(uint8_t i = 0; i < ds18b20Manager.getSensorCount(); i++) {
        if(ds18b20Manager.isSensorValid(i)) {
            int16_t rawTemp = ds18b20Manager.getRawTemperature(i);
            
            // Zapisz temperature jako 16-bit wartość
            uint8_t tempData[2];
            tempData[0] = rawTemp & 0xFF;
            tempData[1] = (rawTemp >> 8) & 0xFF;
            
            flashMemory.writeToMemory(logAddress, tempData, 2);
            logAddress += 2;
            
            // Zapobiegnij przepełnieniu pamięci
            if(logAddress > 0x08015000) {
                logAddress = 0x08014000; // Reset do początku
            }
        }
    }
}

// Funkcja do kalibracji czujnika
void calibrateSensor(uint8_t sensorId, float referenceTemp) {
    float measuredTemp = ds18b20Manager.getTemperature(sensorId);
    float offset = referenceTemp - measuredTemp;
    
    // Zapisz offset kalibracji do pamięci flash
    uint32_t calibrationAddress = 0x08015000 + (sensorId * 4);
    union {
        float f;
        uint32_t u32;
    } data;
    data.f = offset;
    
    uint8_t calibData[4];
    calibData[0] = data.u32 & 0xFF;
    calibData[1] = (data.u32 >> 8) & 0xFF;
    calibData[2] = (data.u32 >> 16) & 0xFF;
    calibData[3] = (data.u32 >> 24) & 0xFF;
    
    flashMemory.writeToMemory(calibrationAddress, calibData, 4);
}

// Funkcja do odczytu skalibrowanej temperatury
float getCalibratedTemperature(uint8_t sensorId) {
    float rawTemp = ds18b20Manager.getTemperature(sensorId);
    
    // Odczytaj offset kalibracji
    uint32_t calibrationAddress = 0x08015000 + (sensorId * 4);
    uint8_t calibData[4];
    flashMemory.readFromMemory(calibrationAddress, calibData, 4);
    
    union {
        float f;
        uint32_t u32;
    } data;
    data.u32 = calibData[0] | (calibData[1] << 8) | (calibData[2] << 16) | (calibData[3] << 24);
    
    return rawTemp + data.f; // Dodaj offset kalibracji
}
