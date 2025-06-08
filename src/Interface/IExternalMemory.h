/*
 * JQBSTM32 Framework - IExternalMemory.h Header
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
#ifndef __IEXTERNALMEMORY_H
#define __IEXTERNALMEMORY_H

//#include "../Interface/IBus.h"

/**
 * @brief Memory address size constants
 * @details Define address size for different memory types
 */
#define ADDRESS_SIZE_8BIT            (0x00000001U) ///< 8-bit address size
#define ADDRESS_SIZE_16BIT           (0x00000002U) ///< 16-bit address size  
#define ADDRESS_SIZE_24BIT           (0x00000003U) ///< 24-bit address size
#define ADDRESS_SIZE_32BIT           (0x00000004U) ///< 32-bit address size

/**
 * @brief External memory interface
 * @details Abstract interface for external memory devices (EEPROM, FRAM, Flash, etc.)
 * @note Provides standardized read/write operations for different memory types
 */
class IExternalMemory {
    public:
        /**
         * @brief Read data from memory
         * @details Reads specified number of bytes from memory starting at given address
         * @param MemAddress Memory address to read from
         * @param pData Pointer to buffer for read data
         * @param Size Number of bytes to read
         */
        virtual void readFromMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) = 0;
        
        /**
         * @brief Write data to memory
         * @details Writes specified number of bytes to memory starting at given address
         * @param MemAddress Memory address to write to
         * @param pData Pointer to data buffer to write
         * @param Size Number of bytes to write
         */
        virtual void writeToMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) = 0;
    protected:
        // IBus *_pInstance; ///< Bus instance pointer (commented out)
};

#endif
