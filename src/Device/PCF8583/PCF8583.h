/*
 * JQBSTM32 Framework - PCF8583.h Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
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
#include "../../Hardware/I2C/I2C.h"
#ifdef __I2C_H_

#ifndef __PCF8583_H_
#define __PCF8583_H_

#include "../../Interface/IExternalMemory.h"

/**
 * @class PCF8583
 * @brief Driver class for PCF8583 Real-Time Clock and Calendar with RAM
 * @details This class provides interface for PCF8583 RTC/Calendar chip via I2C communication.
 *          The PCF8583 includes a real-time clock/calendar, alarm function, and 240 bytes
 *          of static RAM. Features include seconds, minutes, hours, day, date, month, year,
 *          and century with automatic leap year correction.
 * @note Implements IExternalMemory interface for RAM access functionality
 */
class PCF8583 : public IExternalMemory {
    public:
        /**
         * @brief Constructor for PCF8583 RTC driver
         * @param pInstance Pointer to I2C instance for communication
         * @param DevAddress I2C device address of the PCF8583 (default: 0xA0)
         * @param BaseAddress Base address offset for memory operations (default: 0x00)
         * @details Initializes PCF8583 driver with specified I2C interface and addressing.
         *          Device address is determined by A0 pin configuration (0xA0 or 0xA2).
         * @note PCF8583 uses 8-bit addressing for internal registers and RAM
         */
        PCF8583(I2C *pInstance,
            uint16_t DevAddress = 0xA0,
            uint32_t BaseAddress = 0x00
        );
        
        /**
         * @brief Read data from PCF8583 registers or RAM
         * @param MemAddress Register/RAM address to read from (0x00-0xFF)
         * @param pData Pointer to buffer where read data will be stored
         * @param Size Number of bytes to read
         * @details Reads sequential bytes from PCF8583 starting at specified address.
         *          Address range 0x00-0x0F contains RTC registers, 0x10-0xFF contains RAM.
         * @note Time registers use BCD format for easy display conversion
         */
        void readFromMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) override;

        /**
         * @brief Write data to PCF8583 registers or RAM
         * @param MemAddress Register/RAM address to write to (0x00-0xFF)
         * @param pData Pointer to data buffer to write
         * @param Size Number of bytes to write
         * @details Writes sequential bytes to PCF8583 starting at specified address.
         *          Can be used to set time/date or store data in static RAM.
         *          Time registers expect BCD format values.
         * @note Writing to control register (0x00) affects RTC operation mode
         * @warning Incorrect control register values can stop RTC operation
         */
        void writeToMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) override;
        
    protected:
        I2C             *_pInstance;    ///< Pointer to I2C communication interface
        uint16_t        _DevAddress;    ///< I2C device address of the PCF8583
        uint32_t        _BaseAddress;   ///< Base address offset for operations
        uint16_t        _MemAddSize = ADDRESS_SIZE_8BIT; ///< Address size (8-bit for PCF8583)
};
#endif
#endif
