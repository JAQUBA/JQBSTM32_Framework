#include "FlashMemory.h"

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
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, MemAddress, Data) != HAL_OK) {
        HAL_FLASH_Lock();
        return;
    }
    HAL_FLASH_Lock();
}


FLASH_EraseInitTypeDef FlashMemory::GenerateFlashEraseStruct(uint32_t Address) {
    FLASH_EraseInitTypeDef EraseInitStruct;

    #if defined(STM32F1)
        // Dla STM32F1
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;    // Typ operacji (strony)
        EraseInitStruct.PageAddress = Address;                // Adres strony
        EraseInitStruct.NbPages = 1;                          // Liczba stron do wymazania

    #elif defined(STM32F4)
        // Dla STM32F4
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;  // Typ operacji (sektory)
        EraseInitStruct.Sector = GetFlashSector(Address);     // Obliczenie sektora na podstawie adresu
        EraseInitStruct.NbSectors = 1;                        // Liczba sektorów do wymazania
        EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; // Zakres napięcia

    #elif defined(STM32H4)
        // Dla STM32H4
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_MASS;     // Typ operacji (cała pamięć)
        // Można dodać inne pola specyficzne dla H4, jeśli istnieją

    #elif defined(STM32G0)
        // Dla STM32G0
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;    // Typ operacji (strony)
        EraseInitStruct.Page = (Address - FLASH_BASE) / FLASH_PAGE_SIZE; // Numer strony
        EraseInitStruct.NbPages = 1;                         // Liczba stron do wymazania

    #else
        // Domyślny przypadek, jeśli typ procesora nie jest obsługiwany
        #error "Nieobsługiwany typ procesora!"
    #endif

    return EraseInitStruct;
}
#ifdef STM32F4
uint32_t FlashMemory::GetFlashSector(uint32_t Address) {
    
    if (Address < 0x08010000) {
        // Sektory 0–3 (16 KB każdy)
        return (Address - 0x08000000) / 0x4000;
    } else if (Address < 0x08020000) {
        // Sektor 4 (64 KB)
        return FLASH_SECTOR_4;
    } else {
        // Sektory 5–7 (128 KB każdy)
        return FLASH_SECTOR_5 + ((Address - 0x08020000) / 0x20000);
    }
}
#endif