#ifndef __NV_RAM_H_
#define __NV_RAM_H_
#include "../../Core.h"
#include "../../Hardware/I2C/I2C.h"
#include "../../Interface/IExternalMemory.h"

class NV_RAM : public IExternalMemory {
    public:
        NV_RAM(I2C *pInstance, uint8_t address, uint16_t pages, uint16_t pagesize);

        void readFromMemory(uint16_t address, uint16_t *data, uint16_t size, bool blocking = false);
        void writeToMemory(uint16_t address, uint16_t *data, uint16_t size, bool blocking = false);
    
    private:
        // IBus *_pInstance;

        // std::list<_memOperation> functions;
};
#endif