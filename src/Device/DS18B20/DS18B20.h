#include "../../Hardware/OneWire/OneWire.h"

#include "../../Interface/IExternalMemory.h"
#include <functional>

#ifdef ONEWIRE_H

#ifndef __DS18B20_H_
#define __DS18B20_H_

#ifndef DS_MAX_SENSORS
#define DS_MAX_SENSORS 10
#endif


// Kody komend DS18B20
#define DS18B20_READ_ROM        0x33
#define DS18B20_MATCH_ROM       0x55
#define DS18B20_SKIP_ROM        0xCC
#define DS18B20_CONVERT_T       0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

// Struktura przechowująca informacje o czujniku
struct DS18B20Sensor {
    uint64_t romCode;
    float temperature;
    bool isValid;
    uint32_t lastReadTime;
};

// Callback dla zakończonej operacji odczytu temperatury
using TemperatureCallback = std::function<void(uint8_t sensorId, float temperature, bool success)>;

void unpack_rom(uint64_t number, uint8_t *result);
uint64_t pack_rom(uint8_t *buffer);

class DS18B20Manager {
    public:
        DS18B20Manager(OneWire *oneWire, IExternalMemory *flashMemory = nullptr);
        
        // Dodawanie czujników
        bool addSensor(uint8_t id, uint64_t romCode);
        bool addSensorFromFlash(uint8_t id, uint32_t flashAddress);
        void removeSensor(uint8_t id);
        
        // Odczyt ROM z magistrali (dla nowych czujników)
        void readRomFromBus(std::function<void(uint64_t romCode)> callback);
        
        // Operacje na temperaturze
        void startConversion();
        void startConversionForSensor(uint8_t id);
        void readTemperature(uint8_t id, TemperatureCallback callback);
        void readAllTemperatures(TemperatureCallback callback);
        
        // Gettery
        float getTemperature(uint8_t id);
        int16_t getRawTemperature(uint8_t id);
        bool isSensorValid(uint8_t id);
        uint64_t getSensorRom(uint8_t id);
        uint8_t getSensorCount();
          // Automatyczny cykl odczytu
        void enableAutoRead(uint32_t intervalMs = 1000);
        void disableAutoRead();
        void setAutoReadCallback(TemperatureCallback callback, uint32_t intervalMs = 1000);
        
    private:
        OneWire *oneWire;
        IExternalMemory *flashMemory;
        DS18B20Sensor sensors[DS_MAX_SENSORS];
        uint8_t sensorCount;
        
        // Auto-read
        bool autoReadEnabled;
        uint32_t autoReadInterval;
        TemperatureCallback autoReadCallback;
        
        // Stan operacji asynchronicznych
        enum State {
            IDLE,
            READING_ROM,
            CONVERTING,
            READING_SCRATCHPAD
        };
        State currentState;
        uint8_t currentSensorId;
        std::function<void(uint64_t)> romCallback;
        TemperatureCallback tempCallback;
        
        // Bufory
        uint8_t romBuffer[8];
        uint8_t scratchpadBuffer[9];
        
        // Metody pomocnicze
        void processAutoRead();
        void handleRomRead();
        void handleConversionComplete();
        void handleScratchpadRead();
        float calculateTemperature(uint8_t* scratchpad);
        bool isValidRom(uint64_t romCode);
};

// Pojedynczy czujnik DS18B20 (wrapper dla kompatybilności wstecznej)
class DS18B20 {
    public:
        DS18B20(OneWire *oneWire, uint64_t romCode);
        DS18B20(OneWire *oneWire, IExternalMemory *flashMemory, uint32_t flashAddress);
        
        void startConversion();
        void readTemperature(TemperatureCallback callback);
        float getTemperature();
        int16_t getRawTemperature();
        bool isValid();
        uint64_t getRomCode();
        
    private:
        DS18B20Manager* manager;
        uint8_t sensorId;
        bool ownsManager;

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

#endif // __DS18B20_H__
#endif // __ONEWIRE_H__