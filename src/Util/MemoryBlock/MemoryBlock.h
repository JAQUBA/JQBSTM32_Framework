/*
 * JQBSTM32 Framework - MemoryBlock.h Header
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
#ifndef _MEMORYBLOCK_H
#define _MEMORYBLOCK_H
#include "Core.h"
#include "Interface/IExternalMemory.h"

/**
 * @class MemoryBlock
 * @brief Memory block management utility for external memory devices
 * @details This class provides a convenient interface for managing data blocks
 *          in external memory devices like EEPROM, FRAM, or other storage.
 *          Encapsulates read/write operations to specific memory regions
 *          through the IExternalMemory interface.
 * @note Works with any device implementing IExternalMemory interface
 */
class MemoryBlock {
    public:
        /**
         * @brief Constructor for MemoryBlock
         * @param device Pointer to external memory device implementing IExternalMemory interface
         * @param startingAddress Base address in external memory where this block begins
         * @details Initializes memory block manager for specified device and address range.
         *          All subsequent read/write operations will be relative to the starting address.
         * @note Device must be properly initialized before creating MemoryBlock instance
         */
        MemoryBlock(IExternalMemory *device,
         uint32_t startingAddress);
         
        /**
         * @brief Load data from memory block into buffer
         * @param pData Pointer to buffer where loaded data will be stored
         * @param Size Number of bytes to read from memory block
         * @details Reads specified number of bytes from external memory starting at
         *          the configured base address. Data is copied to the provided buffer.
         * @note Buffer must be large enough to hold the requested data size
         * @note Reading beyond device capacity may cause undefined behavior
         */
        void loadBlock(
            uint8_t *pData, 
            uint16_t Size
        );

        /**
         * @brief Save data from buffer to memory block
         * @param pData Pointer to data buffer to write to memory
         * @param Size Number of bytes to write to memory block
         * @details Writes specified number of bytes from buffer to external memory
         *          starting at the configured base address.
         * @note Writing may take time depending on memory type (EEPROM vs FRAM)
         * @note Writing beyond device capacity may cause undefined behavior
         */
        void saveBlock(
            uint8_t *pData, 
            uint16_t Size
        );
        
    private:
        IExternalMemory *_extMemory;     ///< Pointer to external memory device interface
        uint32_t        _startingAddress; ///< Base address for this memory block
};

#endif
