#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include "Core.h"
#include "../../Interface/IExternalMemory.h"

class FlashMemory : public IExternalMemory{
    public:
        void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
        void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
    private:
        FLASH_EraseInitTypeDef GenerateFlashEraseStruct(uint32_t Address);
        #ifdef STM32F4
        uint32_t GetFlashSector(uint32_t Address);
        #endif
};

#endif // FLASH_MEMORY_H
