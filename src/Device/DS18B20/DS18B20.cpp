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

DS18B20::DS18B20(OneWire *oneWire) : ow(oneWire) {
    // Initialize arrays
    for(uint8_t i = 0; i < 9; i++) {
        scratchpad[i] = 0;
    }
    for(uint8_t i = 0; i < 8; i++) {
        b_rom[i] = 0;
    }
}

void DS18B20::unpackRom(uint64_t romAddress, uint8_t *result) {
    // Optimized version using loop (little-endian order for DS18B20)
    for(int i = 7; i >= 0; i--) {
        result[i] = romAddress & 0xFF;
        romAddress >>= 8;
    }
}

uint64_t DS18B20::packRom(uint8_t *buffer) {
    uint64_t value = 0;
    for(int i = 0; i < 8; i++) {
        value = (value << 8) | buffer[i];
    }
    return value;
}

void DS18B20::readTemperature(uint64_t deviceAddress, TemperatureCallback callback) {
    // Safety check
    if(!ow) return;
    
    // Convert ROM address to byte array
    unpackRom(deviceAddress, b_rom);
    
    // Start temperature conversion
    ow->reset();
    uint8_t MATCH_ROM = 0x55;
    ow->transmit(&MATCH_ROM, 1);
    ow->transmit(b_rom, 8);
    uint8_t CMD_CONVERT_T = 0x44;
    ow->transmit(&CMD_CONVERT_T, 1, dataCallback {
        // Schedule temperature reading after conversion time (750ms)
        addTaskMain(taskCallback {
            ow->reset();
            uint8_t MATCH_ROM = 0x55;
            ow->transmit(&MATCH_ROM, 1);
            ow->transmit(b_rom, 8);
            uint8_t CMD_READ_SCRATCHPAD = 0xBE;
            ow->transmitThenReceive(&CMD_READ_SCRATCHPAD, 1, scratchpad, 9, dataCallback {
                // Calculate temperature from scratchpad data
                int16_t rawTemperature = (data[1] << 8) | data[0];
                float temperature = rawTemperature / 16.0f;
                
                // Call the user callback with the result
                if(callback) {
                    callback(deviceAddress, temperature, true);
                }
            });
        }, 750, true); // Execute once after 750ms delay
    });
}

void DS18B20::readSingleDeviceROM(std::function<void(uint64_t, bool)> callbackFn) {
    // Safety check
    if(!ow) return;
    
    ow->reset();
    uint8_t READ_ROM = 0x33;
    
    ow->transmitThenReceive(&READ_ROM, 1, b_rom, 8, [this, callbackFn](uint8_t *data, uint16_t size) {
        uint64_t deviceAddress = packRom(data);
        bool found = (deviceAddress != 0);
        if(callbackFn != nullptr) {
            callbackFn(deviceAddress, found);
        }
    });
}

#endif // __DS18B20_H__