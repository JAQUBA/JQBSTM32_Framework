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
#define DS_MAX_SENSORS 10 ///< Maximum number of DS18B20 sensors supported
#endif

/**
 * @brief Temperature reading callback function type
 * @details Callback function called when temperature reading is complete
 * @param sensorId Sensor identifier
 * @param temperature Temperature value in Celsius
 * @param success Operation success flag
 */
using TemperatureCallback = std::function<void(uint8_t sensorId, float temperature, bool success)>;

/**
 * @brief Unpack ROM code to byte array
 * @details Converts 64-bit ROM code to 8-byte array
 * @param number 64-bit ROM code
 * @param result Pointer to 8-byte result array
 */
void unpack_rom(uint64_t number, uint8_t *result);

/**
 * @brief Pack byte array to ROM code
 * @details Converts 8-byte array to 64-bit ROM code
 * @param buffer Pointer to 8-byte buffer
 * @return uint64_t 64-bit ROM code
 */
uint64_t pack_rom(uint8_t *buffer);

/**
 * @brief DS18B20 temperature sensor driver
 * @details Low-level driver for Dallas/Maxim DS18B20 digital temperature sensor
 * @note Basic implementation for single sensor operations
 */
class DS18B20 {
    public:
        /**
         * @brief DS18B20 constructor
         * @details Initializes DS18B20 driver with OneWire interface
         * @param oneWire Pointer to OneWire interface instance
         */
        DS18B20(OneWire *oneWire);
        
        /**
         * @brief Add sensor with ROM code
         * @details Registers a sensor with its unique ROM code
         * @param romCode 64-bit ROM code of the sensor
         * @param nr Sensor number/identifier
         */
        void addSensor(uint64_t romCode, uint8_t nr);
        
        /**
         * @brief Get temperature reading
         * @details Reads temperature from specified sensor
         * @param id Sensor identifier
         * @return uint16_t Temperature value (raw format)
         */
        uint16_t getTemperature(uint8_t id);
        
        /**
         * @brief Read ROM code from sensor
         * @details Reads ROM code from single sensor on the bus
         */
        void readRom(void);
        
        /**
         * @brief Get ROM code
         * @details Returns the ROM code read from sensor
         * @return uint64_t 64-bit ROM code
         */
        uint64_t getRom(void);
    private:
        bool read_rom = false;        ///< ROM reading status flag
        OneWire *oneWire;            ///< OneWire interface pointer
        uint8_t b_rom[8] ={0};       ///< ROM code byte array
        uint64_t rom;                ///< ROM code as 64-bit value
        uint8_t id=0;                ///< Current sensor ID
        uint8_t b_rd[DS_MAX_SENSORS * 9]={0}; ///< Data buffer for multiple sensors
};

/**
 * @brief DS18B20 Manager class
 * @details High-level manager for multiple DS18B20 temperature sensors
 * @note Provides advanced features like automatic reading and flash storage
 */
class DS18B20Manager {
    public:
        /**
         * @brief DS18B20Manager constructor
         * @details Initializes DS18B20 manager with OneWire interface and optional flash storage
         * @param oneWire Pointer to OneWire interface instance
         * @param flashMemory Pointer to external memory interface for ROM storage (optional)
         */
        DS18B20Manager(OneWire *oneWire, IExternalMemory *flashMemory = nullptr);
        
        /**
         * @brief Add sensor to manager
         * @details Registers a sensor with specific ID and ROM code
         * @param id Sensor identifier (0 to DS_MAX_SENSORS-1)
         * @param romCode 64-bit ROM code of the sensor
         * @return bool Success flag (true if added successfully)
         */
        bool addSensor(uint8_t id, uint64_t romCode);
        
        /**
         * @brief Read temperature asynchronously
         * @details Initiates temperature reading with callback notification
         * @param id Sensor identifier
         * @param callback Callback function called when reading is complete
         */
        void readTemperature(uint8_t id, TemperatureCallback callback);
        
        /**
         * @brief Get last temperature reading
         * @details Returns the last temperature reading for specified sensor
         * @param id Sensor identifier
         * @return float Temperature in Celsius degrees
         */
        float getTemperature(uint8_t id);
        
        /**
         * @brief Set automatic reading callback
         * @details Sets callback for automatic temperature reading
         * @param callback Callback function for temperature updates
         * @param intervalMs Reading interval in milliseconds (default: 1000ms)
         */
        void setAutoReadCallback(TemperatureCallback callback, uint32_t intervalMs = 1000);
        
        /**
         * @brief Enable automatic reading
         * @details Enables automatic temperature reading for all sensors
         * @param intervalMs Reading interval in milliseconds (default: 1000ms)
         */
        void enableAutoRead(uint32_t intervalMs = 1000);
        
        /**
         * @brief Disable automatic reading
         * @details Disables automatic temperature reading
         */
        void disableAutoRead();
        
        /**
         * @brief Check if sensor is valid
         * @details Checks if sensor with specified ID is registered and valid
         * @param id Sensor identifier
         * @return bool True if sensor is valid
         */
        bool isSensorValid(uint8_t id);
        
        /**
         * @brief Get sensor ROM code
         * @details Returns ROM code of specified sensor
         * @param id Sensor identifier
         * @return uint64_t 64-bit ROM code
         */
        uint64_t getSensorRom(uint8_t id);
        
        /**
         * @brief Get sensor count
         * @details Returns number of registered sensors
         * @return uint8_t Number of sensors
         */
        uint8_t getSensorCount();
        
    private:
        OneWire *oneWire;                     ///< OneWire interface pointer
        IExternalMemory *flashMemory;        ///< External memory interface pointer
        DS18B20 *sensors[DS_MAX_SENSORS];    ///< Array of DS18B20 sensor instances
        uint64_t romCodes[DS_MAX_SENSORS];   ///< ROM codes of registered sensors
        bool validSensors[DS_MAX_SENSORS];   ///< Validity flags for sensors
        uint8_t sensorCount;                 ///< Number of registered sensors
        TemperatureCallback autoCallback;     ///< Automatic reading callback
        bool autoReadEnabled;                ///< Automatic reading enable flag
};

#endif // __DS18B20_H__
#endif // __ONEWIRE_H__