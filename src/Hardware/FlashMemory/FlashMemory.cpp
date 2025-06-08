/*
 * JQBSTM32 Framework - FlashMemory.cpp Implementation
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
#include "FlashMemory.h"
#ifdef HAL_FLASH_MODULE_ENABLED

void FlashMemory::writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size) {
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct = GenerateFlashEraseStruct(MemAddress);

    uint32_t PageError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return;
    }
    for (uint16_t i = 0; i < Size; i += 8) {
        uint64_t data = 0;
        memcpy(&data, &pData[i], (Size - i >= 8) ? 8 : Size - i);
        #ifdef STM32H7
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, MemAddress + i, data) != HAL_OK) {
            HAL_FLASH_Lock();
            return;
        }
        #elif defined(STM32F4) || defined(STM32F1) || defined(STM32G0) || defined(STM32G4) || defined(STM32L4)
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, MemAddress + i, data) != HAL_OK) {
            HAL_FLASH_Lock();
            return;
        }
        #endif
    }
    HAL_FLASH_Lock();
}

void FlashMemory::readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size) {
    for (uint16_t i = 0; i < Size; i++) {
        pData[i] = *reinterpret_cast<uint8_t*>(MemAddress + i);
    }
}

uint64_t FlashMemory::read(uint32_t MemAddress) {
    return *reinterpret_cast<uint64_t*>(MemAddress);
}

void FlashMemory::write(uint32_t MemAddress, uint64_t Data) {
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct = GenerateFlashEraseStruct(MemAddress);

    uint32_t PageError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return;
    }
    #ifdef STM32H7
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, MemAddress, Data) != HAL_OK) {
        HAL_FLASH_Lock();
        return;
    }
    #elif defined(STM32F4) || defined(STM32F1) || defined(STM32G0) || defined(STM32G4) || defined(STM32L4)
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, MemAddress, Data) != HAL_OK) {
        HAL_FLASH_Lock();
        return;
    }
    #endif
    HAL_FLASH_Lock();
}


FLASH_EraseInitTypeDef FlashMemory::GenerateFlashEraseStruct(uint32_t Address) {
    FLASH_EraseInitTypeDef EraseInitStruct;

    #if defined(STM32F1)
        // For STM32F1
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;    // Operation type (pages)
        EraseInitStruct.PageAddress = Address;                // Page address
        EraseInitStruct.NbPages = 1;                          // Number of pages to erase

    #elif defined(STM32F4)
        // For STM32F4
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;  // Operation type (sectors)
        EraseInitStruct.Sector = GetFlashSector(Address);     // Sector calculation based on address
        EraseInitStruct.NbSectors = 1;                        // Number of sectors to erase
        EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; // Voltage range

    #elif defined(STM32H4)
        // For STM32H4
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_MASS;     // Operation type (entire memory)
        // Other H4-specific fields can be added if they exist

    #elif defined(STM32G0)
        // For STM32G0
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;    // Operation type (pages)
        EraseInitStruct.Page = (Address - FLASH_BASE) / FLASH_PAGE_SIZE; // Page number
        EraseInitStruct.NbPages = 1;                          // Number of pages to erase

    #elif defined(STM32G4)
        // For STM32G4
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;    // Operation type (pages)
        EraseInitStruct.Page = (Address - FLASH_BASE) / FLASH_PAGE_SIZE; // Page number
        EraseInitStruct.NbPages = 1;                          // Number of pages to erase

    #elif defined(STM32L4)
        // For STM32L4
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;    // Operation type (pages)
        EraseInitStruct.Page = (Address - FLASH_BASE) / FLASH_PAGE_SIZE; // Page number
        EraseInitStruct.NbPages = 1;                          // Number of pages to erase
        EraseInitStruct.Banks = FLASH_BANK_1;                 // Bank setting, if needed
    #elif defined(STM32H7)
        // For STM32L5
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;  // Operation type (sectors)
        EraseInitStruct.Sector = GetFlashSector(Address);     // Sector calculation based on address
        EraseInitStruct.Banks = FLASH_BANK_1;                 // Bank setting, if needed
        EraseInitStruct.NbSectors = 1;                        // Number of sectors to erase
        EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; // Voltage range
    #else
        // Default case, if processor type is not supported
        #error "Unsupported processor type!"
    #endif

    return EraseInitStruct;
}

#if defined(STM32F4) || defined(STM32H7)
uint32_t FlashMemory::GetFlashSector(uint32_t Address) {
    
    if (Address < 0x08010000) {
        // Sectors 0-3 (16 KB each)
        return (Address - 0x08000000) / 0x4000;
    } else if (Address < 0x08020000) {
        // Sector 4 (64 KB)
        return FLASH_SECTOR_4;
    } else {
        // Sectors 5-11 (128 KB each)
        return FLASH_SECTOR_5 + ((Address - 0x08020000) / 0x20000);
    }
}
#endif
#endif
