#include "FlashMemory.h"

// Konstruktor
FlashMemory::FlashMemory() {}

// Funkcja do zapisu danych do pamięci flash
void FlashMemory::writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size) {
    HAL_FLASH_Unlock(); // Odblokowanie pamięci flash

    // Kasowanie strony pamięci flash, na której będą zapisane dane
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;
    uint32_t page = (MemAddress - FLASH_BASE) / flashPageSize; // Obliczenie numeru strony

    // FLASH_PAGE_NB; // Liczba stron pamięci flash

    EraseInitStruct.NbPages = 1; // Liczba stron do wymazania
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = page; // Numer strony do wymazania
    EraseInitStruct.Banks = FLASH_BANK_1; // STM32G071 ma tylko jeden bank flash

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return; // Wyjście w przypadku błędu
    }

    // Zapis danych do pamięci flash, 64-bitowymi słowami (Double Word)
    for (uint16_t i = 0; i < Size; i += 8) {
        uint64_t data = 0;
        memcpy(&data, &pData[i], (Size - i >= 8) ? 8 : Size - i); // Kopiowanie danych 64-bitowych
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, MemAddress + i, data) != HAL_OK) {
            HAL_FLASH_Lock();
            return; // Wyjście w przypadku błędu
        }
    }

    HAL_FLASH_Lock(); // Zablokowanie pamięci flash po zakończeniu operacji
}

// Funkcja do odczytu danych z pamięci flash
void FlashMemory::readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size) {
    for (uint16_t i = 0; i < Size; i++) {
        pData[i] = *reinterpret_cast<uint8_t*>(MemAddress + i); // Odczyt danych bajt po bajcie
    }
}
