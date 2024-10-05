#include "FlashMemory.h"

void FlashMemory::writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size) {
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;

#ifdef STM32F1
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = 0x08000000;
    EraseInitStruct.NbPages = 1;

#elif defined(STM32F4)
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = FLASH_SECTOR_2;
    EraseInitStruct.NbSectors = 1;

#elif defined(STM32G0)
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.Page = (MemAddress - FLASH_BASE) / FLASH_PAGE_SIZE;;
    EraseInitStruct.NbPages = 1;

#elif defined(STM32H7)
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.Sector = FLASH_SECTOR_2;
    EraseInitStruct.NbSectors = 1;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_4;
    
// Dodaj inne rodziny STM32, jeśli potrzebne
#else
    #error "Nieznana rodzina STM32"
#endif

    // EraseInitStruct.NbPages = 1;
    // EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    // EraseInitStruct.PageAddress = page;
    // EraseInitStruct.Banks = FLASH_BANK_1;
    uint32_t PageError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return;
    }

    for (uint16_t i = 0; i < Size; i += 8) {
        uint64_t data = 0;
        memcpy(&data, &pData[i], (Size - i >= 8) ? 8 : Size - i);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, MemAddress + i, data) != HAL_OK) {
            HAL_FLASH_Lock();
            return;
        }
    }
    HAL_FLASH_Lock();
}

void FlashMemory::readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size) {
    for (uint16_t i = 0; i < Size; i++) {
        pData[i] = *reinterpret_cast<uint8_t*>(MemAddress + i);
    }
}
