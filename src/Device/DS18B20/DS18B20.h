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
#include <functional>

#ifdef ONEWIRE_H

#ifndef __DS18B20_H_
#define __DS18B20_H_

/**
 * @brief DS18B20 temperature sensor driver
 * @details Simple driver for Dallas/Maxim DS18B20 digital temperature sensor
 */
class DS18B20 {
    public:        /**
         * @brief DS18B20 constructor
         * @details Initializes DS18B20 driver with OneWire interface
         * @param oneWire Pointer to OneWire interface instance
         */
        DS18B20(OneWire *oneWire);
          /**
         * @brief Read temperature asynchronously
         * @details Initiates temperature reading with callback notification
         * @param deviceAddress 64-bit ROM address of the sensor
         * @param callback Callback function called when reading is complete
         */
        void readTemperature(uint64_t deviceAddress, const std::function<void(float temperature)> &callbackFn);

        /**
         * @brief Read single device ROM address
         * @details Reads ROM address of a single device on the bus using READ_ROM command
         * @param callbackFn Callback function called with device address and success flag
         * @note This function only works when there is exactly one device on the bus
         */
        void readSingleDeviceROM(const std::function<void(uint64_t, bool)> &callbackFn);
        
    private:
        OneWire *ow;                    ///< OneWire interface pointer
        
        /**
         * @brief Convert 64-bit ROM address to byte array
         * @param romAddress 64-bit ROM address
         * @param result Pointer to 8-byte result array
         */
        void unpackRom(uint64_t romAddress, uint8_t *result);
        
        /**
         * @brief Convert byte array to 64-bit ROM address
         * @param buffer Pointer to 8-byte ROM address buffer
         * @return uint64_t Packed ROM address
         */
        uint64_t packRom(uint8_t *buffer);
};

#endif // __DS18B20_H__
#endif // __ONEWIRE_H__
