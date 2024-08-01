#ifndef _MEMORYBLOCK_H
#define _MEMORYBLOCK_H
#include "Core.h"
#include "Interface/IExternalMemory.h"


class MemoryBlock {
    public:
        MemoryBlock(IExternalMemory *device, uint32_t startingAddress);
        void loadBlock(
            uint8_t *pData, 
            uint16_t Size
        );

        void saveBlock(
            uint8_t *pData, 
            uint16_t Size
        );
    private:
        IExternalMemory *_extMemory;
        uint32_t        _startingAddress;
};

#endif