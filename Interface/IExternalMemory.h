#ifndef __IEXTERNALMEMORY_H
#define __IEXTERNALMEMORY_H

#include "../Interface/IBus.h"

class IExternalMemory {
    public:
        virtual void readFromMemory(uint16_t MemAddress, uint16_t *pData, uint16_t Size, bool Blocking = false) = 0;
        virtual void writeToMemory(uint16_t MemAddress, uint16_t *pData, uint16_t Size, bool Blocking = false) = 0;
    protected:
        IBus *_pInstance;
};

#endif