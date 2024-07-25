#ifndef __IEXTERNALMEMORY_H
#define __IEXTERNALMEMORY_H

#include "../Interface/IBus.h"

#define MEMADD_SIZE_8BIT            (0x00000001U)
#define MEMADD_SIZE_16BIT           (0x00000002U)
#define MEMADD_SIZE_32BIT           (0x00000004U)

class IExternalMemory {
    public:
        virtual void readFromMemory(
            uint32_t MemAddress,
            uint16_t MemAddSize, 
            uint8_t *pData, 
            uint16_t Size
        ) = 0;
        virtual void writeToMemory(
            uint32_t MemAddress,
            uint16_t MemAddSize, 
            uint8_t *pData, 
            uint16_t Size
        ) = 0;
        
        uint16_t _MemAddSize; 
    protected:
        IBus *_pInstance;
};

#endif