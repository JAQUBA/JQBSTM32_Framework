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
#include <cstdlib>

#ifdef __DS18B20_H_

DS18B20::DS18B20(OneWire *oneWire) : ow(oneWire) {
    // Constructor - no array initialization needed
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

void DS18B20::readTemperature(uint64_t deviceAddress, const std::function<void(float temperature)> &callbackFn) {
    // Safety check
    if(!ow) return;
    
    // Allocate local ROM buffer
    uint8_t *b_rom = (uint8_t*)malloc(8);
    if(!b_rom) return; // Failed to allocate memory
    
    // Convert ROM address to byte array
    unpackRom(deviceAddress, b_rom);

    ow->reset();

    // Start temperature conversion
    uint8_t cmd_buf[10];
    cmd_buf[0] = 0x55; // MATCH_ROM
    for(int i = 0; i < 8; ++i) cmd_buf[1 + i] = b_rom[i];
    cmd_buf[9] = 0x44; // CMD_CONVERT_T
    ow->transmit(cmd_buf, 10, [=](uint8_t *data, uint16_t size) {
        // Schedule temperature reading after conversion time (750ms)
        addTaskMain([=](taskStruct *task) {
            // Allocate scratchpad buffer for this operation
            uint8_t *scratchpad = (uint8_t*)malloc(9);
            if(!scratchpad) {
                free(b_rom);
                return; // Failed to allocate memory
            }

            ow->reset();
            
            // Combine all commands into a single buffer for efficiency
            uint8_t cmd_buf2[10];
            cmd_buf2[0] = 0x55; // MATCH_ROM
            for(int i = 0; i < 8; ++i) cmd_buf2[1 + i] = b_rom[i];
            cmd_buf2[9] = 0xBE; // CMD_READ_SCRATCHPAD
            ow->transmitThenReceive(cmd_buf2, 10, scratchpad, 9, [=](uint8_t *data, uint16_t size) {
                // Calculate temperature from scratchpad data
                int16_t rawTemperature = (data[1] << 8) | data[0];
                float temperature = rawTemperature / 16.0f;
                
                // Call the user callback with the result
                if(callbackFn) {
                    callbackFn(temperature);
                }
                
                // Free allocated memory
                free(scratchpad);
                free(b_rom);
            });
        }, 750, true); // Execute once after 750ms delay
    });
}

void DS18B20::readSingleDeviceROM(const std::function<void(uint64_t, bool)> &callbackFn) {
    // Safety check
    if(!ow) return;
    
    // Allocate local ROM buffer
    uint8_t *b_rom = (uint8_t*)malloc(8);
    if(!b_rom) return; // Failed to allocate memory
    
    ow->reset();
    uint8_t READ_ROM = 0x33;
    
    ow->transmitThenReceive(&READ_ROM, 1, b_rom, 8, [=](uint8_t *data, uint16_t size) {
        // Copy ROM data to local buffer
        for(int i = 0; i < 8; i++) {
            b_rom[i] = data[i];
        }
        uint64_t deviceAddress = packRom(b_rom);
        bool found = (deviceAddress != 0);
        if(callbackFn) {
            callbackFn(deviceAddress, found);
        }
        
        // Free allocated memory
        free(b_rom);
    });
}

#endif // __DS18B20_H__