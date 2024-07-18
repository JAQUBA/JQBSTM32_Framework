#ifndef __NV_RAM_H_
#define __NV_RAM_H_
#include "../../Core.h"
#include "../../Hardware/I2C/I2C.h"
#include "../../Interface/IExternalMemory.h"

class NV_RAM : public IExternalMemory {
    public:
        NV_RAM(I2C *pInstance,
            uint16_t DevAddress,
            uint32_t BaseAddress);
        
        void readFromMemory(
            uint32_t MemAddress, 
            uint8_t *pData, 
            uint16_t Size
        ) override;

        void writeToMemory(
            uint32_t MemAddress, 
            uint8_t *pData, 
            uint16_t Size
        ) override;

    protected:
        uint16_t        _MemAdress;
        uint16_t        _size;

        I2C             *_pInstanceI2C;
        uint16_t        _DevAddress;
        
};
#endif