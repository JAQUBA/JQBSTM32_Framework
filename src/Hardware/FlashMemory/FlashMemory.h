/*
 * JQBSTM32 Framework - FlashMemory.h Header
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
#include "../../Core.h"
#ifdef HAL_FLASH_MODULE_ENABLED

#ifndef __FLASHMEMORY_H_
#define __FLASHMEMORY_H_

#include "../../Interface/IExternalMemory.h"

/**
 * @brief Internal Flash Memory class
 * @details Provides access to STM32 internal flash memory for data storage
 * @note Inherits from IExternalMemory interface for standardized memory operations
 */
class FlashMemory : public IExternalMemory{
    public:
        /**
         * @brief Write data to flash memory
         * @details Writes specified data to flash memory at given address
         * @param MemAddress Memory address to write to
         * @param pData Pointer to data buffer to write
         * @param Size Number of bytes to write
         */
        void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
        
        /**
         * @brief Read data from flash memory
         * @details Reads specified number of bytes from flash memory
         * @param MemAddress Memory address to read from
         * @param pData Pointer to buffer for read data
         * @param Size Number of bytes to read
         */
        void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);

        /**
         * @brief Read 64-bit value from flash
         * @details Reads a 64-bit value from specified flash address
         * @param MemAddress Memory address to read from
         * @return uint64_t 64-bit value read from flash
         */
        uint64_t read(uint32_t MemAddress);
        
        /**
         * @brief Write 64-bit value to flash
         * @details Writes a 64-bit value to specified flash address
         * @param MemAddress Memory address to write to
         * @param Data 64-bit data value to write
         */
        void write(uint32_t MemAddress, uint64_t Data);
        
    private:
        /**
         * @brief Generate flash erase structure
         * @details Creates flash erase initialization structure for specified address
         * @param Address Flash address for erase operation
         * @return FLASH_EraseInitTypeDef Flash erase configuration structure
         */
        FLASH_EraseInitTypeDef GenerateFlashEraseStruct(uint32_t Address);
        
        #if defined(STM32F4) || defined(STM32H7)
        /**
         * @brief Get flash sector for address
         * @details Returns flash sector number for specified address (STM32F4/H7 specific)
         * @param Address Flash address
         * @return uint32_t Flash sector number
         */
        uint32_t GetFlashSector(uint32_t Address);
        #endif
};

#endif
#endif
