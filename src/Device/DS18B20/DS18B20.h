/*
 * JQBSTM32 Framework - DS18B20 Temperature Sensor Driver Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * Driver for Dallas/Maxim DS18B20 digital temperature sensor.
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 */

#include "../../Hardware/OneWire/OneWire.h"
#include <functional>
#include <queue>

#ifdef ONEWIRE_H

#ifndef __DS18B20_H_
#define __DS18B20_H_

/**
 * @brief DS18B20 temperature sensor driver
 * @details Simple driver for Dallas/Maxim DS18B20 digital temperature sensor
 */
class DS18B20 {
    public:        /**
         * @brief Supported sensor resolutions
         */
        enum class Resolution : uint8_t {
            BITS_9 = 9,
            BITS_10 = 10,
            BITS_11 = 11,
            BITS_12 = 12
        };

        /**
         * @brief DS18B20 constructor
         * @details Initializes DS18B20 driver with OneWire interface
         * @param oneWire Pointer to OneWire interface instance
         * @param resolution Sensor resolution (default: 11 bits)
         */
        DS18B20(OneWire *oneWire, Resolution resolution = Resolution::BITS_11);
          /**
         * @brief Read temperature asynchronously
         * @details Initiates temperature reading with callback notification
         * @param deviceAddress 64-bit ROM address of the sensor
         * @param callback Callback function called when reading is complete
         */
        void readTemperature(uint64_t deviceAddress, const std::function<void(float temperature)> &callbackFn);

        /**
         * @brief Read temperature asynchronously and report whether the measurement is valid
         * @param deviceAddress 64-bit ROM address of the sensor
         * @param callbackFn Callback called with success and temperature (temperature is ignored on failure)
         */
        void readTemperatureWithStatus(uint64_t deviceAddress, const std::function<void(bool success, float temperature)> &callbackFn);

        /**
         * @brief Read single device ROM address
         * @details Reads ROM address of a single device on the bus using READ_ROM command
         * @param callbackFn Callback function called with device address and success flag
         * @note This function only works when there is exactly one device on the bus
         */
        void readSingleDeviceROM(const std::function<void(uint64_t, bool)> &callbackFn);
        
    private:
        enum class RequestType : uint8_t {
            TEMPERATURE,
            ROM
        };

        struct Request {
            RequestType type;
            uint8_t rom[8];
            uint8_t scratchpad[9];
            std::function<void(bool, float)> temperatureStatusCallback;
            std::function<void(uint64_t, bool)> romCallback;
        };

        OneWire *ow;                    ///< OneWire interface pointer
        const Resolution resolution;
        std::queue<Request *> requests;
        Request *activeRequest = nullptr;
        bool dispatchingCallback = false;

        void enqueueRequest(Request *request);
        void startNextRequest();
        void startActiveRequest();
        void startTemperatureRequest();
        void startRomRequest();
        void readTemperatureScratchpad();
        void finishActiveRequest(bool success, float temperature = 0.0f);
        uint32_t conversionTimeMs() const;
        
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
