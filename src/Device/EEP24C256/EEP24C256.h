/*
 * JQBSTM32 Framework - EEP24C256.h Header
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

#ifndef __EEP24C256_H_
#define __EEP24C256_H_

#include "../../Interface/IExternalMemory.h"

/**
 * @class EEP24C256
 * @brief Driver class for 24C256 EEPROM memory device
 * @details This class provides interface for reading and writing data to 24C256 EEPROM
 *          via I2C communication. The 24C256 is a 256Kbit (32KB) serial EEPROM
 *          organized as 32,768 x 8 bits. Implements IExternalMemory interface for
 *          standardized memory operations.
 * @note Supports page write operations (64 bytes per page) and 16-bit addressing
 */
class EEP24C256 : public IExternalMemory {
    public:
        /**
         * @brief Constructor for EEP24C256 EEPROM driver
         * @param pInstance Pointer to I2C instance for communication
         * @param DevAddress I2C device address of the EEPROM (default: 0xA0)
         * @param BaseAddress Base memory address offset (default: 0x00)
         * @details Initializes EEPROM driver with specified I2C interface and addressing.
         *          Device address should match the hardware configuration (A0, A1, A2 pins).
         * @note 24C256 uses 16-bit memory addressing (32KB total capacity)
         */
        EEP24C256(I2C *pInstance,
            uint16_t DevAddress = 0xA0,
            uint32_t BaseAddress = 0x00
        );
        
        /**
         * @brief Read data from EEPROM memory
         * @param MemAddress Memory address to read from (0-32767 for 24C256)
         * @param pData Pointer to buffer where read data will be stored
         * @param Size Number of bytes to read
         * @details Reads sequential bytes from EEPROM starting at specified address.
         *          Supports reading across page boundaries with automatic address increment.
         * @note Reading beyond device capacity will wrap around to address 0
         */
        void readFromMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) override;

        /**
         * @brief Write data to EEPROM memory
         * @param MemAddress Memory address to write to (0-32767 for 24C256)
         * @param pData Pointer to data buffer to write
         * @param Size Number of bytes to write
         * @details Writes sequential bytes to EEPROM starting at specified address.
         *          Handles page write operations automatically (64-byte pages for 24C256).
         *          Write operations may take 5-10ms per page to complete.
         * @note Writing beyond device capacity will wrap around to address 0
         * @warning Excessive write cycles can wear out EEPROM (typical endurance: 1M cycles)
         */
        void writeToMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) override;
        
    protected:
        I2C             *_pInstance;    ///< Pointer to I2C communication interface
        uint16_t        _DevAddress;    ///< I2C device address of the EEPROM
        uint32_t        _BaseAddress;   ///< Base address offset for memory operations
        uint16_t        _MemAddSize = ADDRESS_SIZE_16BIT; ///< Memory address size (16-bit for 24C256)
};
#endif
#endif
