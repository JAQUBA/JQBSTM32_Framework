/*
 * JQBSTM32 Framework - DS18B20 Temperature Sensor Driver Implementation
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

#include "DS18B20.h"
#include "../../Core.h"
#ifdef __DS18B20_H_

uint64_t sensors[DS_MAX_SENSORS] = {0};

bool readT = false;

void unpack_rom(uint64_t number, uint8_t *result) {
    result[7] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[6] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[5] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[4] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[3] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[2] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[1] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[0] = number & 0x00000000000000FF ;
}

uint64_t  pack_rom(uint8_t *buffer) {

    uint64_t value ;

    value = buffer[0] ;
    value = (value << 8 ) + buffer[1] ;
    value = (value << 8 ) + buffer[2] ;
    value = (value << 8 ) + buffer[3] ;
    value = (value << 8 ) + buffer[4] ;
    value = (value << 8 ) + buffer[5] ;
    value = (value << 8 ) + buffer[6] ;
    value = (value << 8 ) + buffer[7] ;

    return value ;
}

DS18B20::DS18B20(OneWire *a_oneWire) {
    this->oneWire = a_oneWire;
    addTaskMain(taskCallback {
        if (readT) {
            if (!read_rom) {                for (id=0; id<DS_MAX_SENSORS; id++) {
                    unpack_rom(sensors[id], b_rom);
                    if (b_rom[id]>0) {
                        oneWire->transaction(0x55, b_rom, 0xBE, nullptr, 0, b_rd+(id*9), 9); //temperature reading
                    }
                }
            }else {//read rom
                uint8_t rom_com=0x33;
	            oneWire->transmitThenReceive(&rom_com, 1, b_rom, 8);//read rom !!!
            }
            readT = false;
        } else {
            if (read_rom) {
                read_rom = false;
                rom = pack_rom(b_rom);            } else {
                readT = true;
                oneWire->transaction(0xCC, nullptr, 0x44);//start temp. conversion
            }
        }
	}, 1000);
}

void DS18B20::readRom(void) {
    read_rom = true;
}

uint64_t DS18B20::getRom(void) {
    return rom;
}

void DS18B20::addSensor(uint64_t romCode, uint8_t id) {
    sensors[id]=romCode;
}

uint16_t DS18B20::getTemperature(uint8_t id) {
    return    (*(b_rd+(id*9)+1))<<8 | *(b_rd+(id*9));
}

// ===== DS18B20Manager Implementation =====

DS18B20Manager::DS18B20Manager(OneWire *oneWire, IExternalMemory *flashMemory) 
    : oneWire(oneWire), flashMemory(flashMemory), sensorCount(0), autoReadEnabled(false) {
    
    // Array initialization
    for(uint8_t i = 0; i < DS_MAX_SENSORS; i++) {
        sensors[i] = nullptr;
        romCodes[i] = 0;
        validSensors[i] = false;
    }
}

bool DS18B20Manager::addSensor(uint8_t id, uint64_t romCode) {
    if(id >= DS_MAX_SENSORS) return false;
    
    if(sensors[id] == nullptr) {
        sensors[id] = new DS18B20(oneWire);
    }
    
    sensors[id]->addSensor(romCode, id);
    romCodes[id] = romCode;
    validSensors[id] = true;
    
    if(id >= sensorCount) {
        sensorCount = id + 1;
    }
    
    return true;
}

void DS18B20Manager::readTemperature(uint8_t id, TemperatureCallback callback) {
    if(id >= DS_MAX_SENSORS || !validSensors[id] || sensors[id] == nullptr) {
        if(callback) callback(id, 0.0f, false);
        return;
    }
    
    // Get raw value and convert to float
    uint16_t rawTemp = sensors[id]->getTemperature(id);
    float temperature = (float)rawTemp / 16.0f; // DS18B20 has 0.0625°C resolution
    
    if(callback) callback(id, temperature, true);
}

float DS18B20Manager::getTemperature(uint8_t id) {
    if(id >= DS_MAX_SENSORS || !validSensors[id] || sensors[id] == nullptr) {
        return 0.0f;
    }
    
    uint16_t rawTemp = sensors[id]->getTemperature(id);
    return (float)rawTemp / 16.0f;
}

void DS18B20Manager::setAutoReadCallback(TemperatureCallback callback, uint32_t intervalMs) {
    autoCallback = callback;
    
    if(callback && !autoReadEnabled) {
        enableAutoRead(intervalMs);
    }
}

void DS18B20Manager::enableAutoRead(uint32_t intervalMs) {
    autoReadEnabled = true;
    
    addTaskMain(taskCallback {
        if(autoReadEnabled && autoCallback) {
            for(uint8_t i = 0; i < sensorCount; i++) {
                if(validSensors[i]) {
                    readTemperature(i, autoCallback);
                }
            }
        }
    }, intervalMs);
}

void DS18B20Manager::disableAutoRead() {
    autoReadEnabled = false;
}

bool DS18B20Manager::isSensorValid(uint8_t id) {
    return id < DS_MAX_SENSORS && validSensors[id];
}

uint64_t DS18B20Manager::getSensorRom(uint8_t id) {
    if(id >= DS_MAX_SENSORS || !validSensors[id]) {
        return 0;
    }
    return romCodes[id];
}

uint8_t DS18B20Manager::getSensorCount() {
    return sensorCount;
}

#endif // __DS18B20_H__