#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include "Core.h"
#ifdef HAL_FLASH_MODULE_ENABLED
#include "../../Interface/IExternalMemory.h"

class FlashMemory : public IExternalMemory{
    public:
        void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
        void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);

        uint64_t read(uint32_t MemAddress);
        void write(uint32_t MemAddress, uint64_t Data);
        
    private:
        FLASH_EraseInitTypeDef GenerateFlashEraseStruct(uint32_t Address);
        #ifdef STM32F4
        uint32_t GetFlashSector(uint32_t Address);
        #endif
};

#endif // FLASH_MEMORY_H
#endif