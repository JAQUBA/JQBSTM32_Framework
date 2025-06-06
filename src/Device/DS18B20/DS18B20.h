#include "../../Hardware/OneWire/OneWire.h"
#include "../../Interface/IExternalMemory.h"
#include <functional>

#ifdef ONEWIRE_H

#ifndef __DS18B20_H_
#define __DS18B20_H_

#ifndef DS_MAX_SENSORS
#define DS_MAX_SENSORS 10
#endif

// Callback dla zakończonej operacji odczytu temperatury
using TemperatureCallback = std::function<void(uint8_t sensorId, float temperature, bool success)>;

void unpack_rom(uint64_t number, uint8_t *result);
uint64_t pack_rom(uint8_t *buffer);

class DS18B20 {
    public:
        DS18B20(OneWire *oneWire);
        void addSensor(uint64_t romCode, uint8_t nr);
        uint16_t getTemperature(uint8_t id);
        void readRom(void);
        uint64_t getRom(void);
    private:
        bool read_rom = false;
        OneWire *oneWire;
        uint8_t b_rom[8] ={0};
        uint64_t rom;
        uint8_t id=0;
        uint8_t b_rd[DS_MAX_SENSORS * 9]={0};
};

// Prosta implementacja DS18B20Manager jako wrapper dla kompatybilności
class DS18B20Manager {
    public:
        DS18B20Manager(OneWire *oneWire, IExternalMemory *flashMemory = nullptr);
        
        // Dodawanie czujników
        bool addSensor(uint8_t id, uint64_t romCode);
        
        // Odczyt temperatury
        void readTemperature(uint8_t id, TemperatureCallback callback);
        float getTemperature(uint8_t id);
        
        // Automatyczny odczyt
        void setAutoReadCallback(TemperatureCallback callback, uint32_t intervalMs = 1000);
        void enableAutoRead(uint32_t intervalMs = 1000);
        void disableAutoRead();
        
        // Gettery
        bool isSensorValid(uint8_t id);
        uint64_t getSensorRom(uint8_t id);
        uint8_t getSensorCount();
        
    private:
        OneWire *oneWire;
        IExternalMemory *flashMemory;
        DS18B20 *sensors[DS_MAX_SENSORS];
        uint64_t romCodes[DS_MAX_SENSORS];
        bool validSensors[DS_MAX_SENSORS];        uint8_t sensorCount;
        TemperatureCallback autoCallback;
        bool autoReadEnabled;
};

#endif // __DS18B20_H__
#endif // __ONEWIRE_H__