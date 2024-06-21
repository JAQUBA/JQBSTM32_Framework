#ifndef __IEXTERNALMEMORY_H
#define __IEXTERNALMEMORY_H

#include "../Interface/IBus.h"

class IExternalMemory {
    public:
        struct memOperation {
            enum {
                OK,
                TIMEOUT,
                ERROR
            } status;
        };

        virtual void readFromMemory(uint16_t MemAddress, uint8_t *pData, uint16_t Size, std::function<void(memOperation *operation)> functionPointer = nullptr) = 0;
        virtual void writeToMemory(uint16_t MemAddress, uint8_t *pData, uint16_t Size, std::function<void(memOperation *operation)> functionPointer = nullptr) = 0;
    protected:
        // IBus *_pInstance;
};

#endif