#ifndef __EEP24C04_H_
#define __EEP24C04_H_
#include "../../Core.h"
#include "../../Hardware/I2C/I2C.h"
#include "../../Interface/IExternalMemory.h"

class EEP24C04 : public IExternalMemory {
    public:
        EEP24C04(I2C *pInstance,
            uint16_t DevAddress = 0xA0,
            uint32_t BaseAddress = 0x00
        );
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
        I2C             *_pInstance;
        uint16_t        _DevAddress;
        uint32_t        _BaseAddress;
        uint16_t        _MemAddSize = ADDRESS_SIZE_8BIT;
};
#endif