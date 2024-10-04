#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include "Core.h"
#include "../../Interface/IExternalMemory.h"

class FlashMemory : public IExternalMemory{
public:
    // Konstruktor
    FlashMemory();

    // Funkcja do zapisu danych do pamięci flash
    void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);

    // Funkcja do odczytu danych z pamięci flash
    void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);

private:
    static constexpr uint32_t flashPageSize = 2048; // Rozmiar strony pamięci flash (STM32G071)
};

#endif // FLASH_MEMORY_H
