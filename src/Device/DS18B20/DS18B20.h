/*
 * JQBSTM32 Framework - DS18B20 Temperature Sensor Driver Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * Driver for Dallas/Maxim DS18B20 digital temperature sensor.
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

#include "../../Hardware/OneWire/OneWire.h"
#include "../../Interface/IExternalMemory.h"
#include <functional>

#ifdef ONEWIRE_H

#ifndef __DS18B20_H_
#define __DS18B20_H_

#ifndef DS_MAX_SENSORS
#define DS_MAX_SENSORS 10
#endif

// Callback for completed temperature read operation
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

// Simple DS18B20Manager implementation as wrapper for compatibility
class DS18B20Manager {
    public:
        DS18B20Manager(OneWire *oneWire, IExternalMemory *flashMemory = nullptr);
        
        // Adding sensors
        bool addSensor(uint8_t id, uint64_t romCode);
        
        // Temperature reading
        void readTemperature(uint8_t id, TemperatureCallback callback);
        float getTemperature(uint8_t id);
        
        // Automatic reading
        void setAutoReadCallback(TemperatureCallback callback, uint32_t intervalMs = 1000);
        void enableAutoRead(uint32_t intervalMs = 1000);
        void disableAutoRead();
        
        // Getters
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