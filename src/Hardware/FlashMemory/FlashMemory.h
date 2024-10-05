#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include "Core.h"
#include "../../Interface/IExternalMemory.h"

class FlashMemory : public IExternalMemory{
public:
    void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
    void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
};

#endif // FLASH_MEMORY_H
