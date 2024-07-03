#ifndef __EEPROM_H_
#define __EEPROM_H_
#include "../../Core.h"
#include "../../Hardware/I2C/I2C.h"
#include "../../Interface/IExternalMemory.h"

class EEPROM : public IExternalMemory {
    public:
        EEPROM(I2C *pInstance, uint16_t DevAddress, uint16_t pages = 1, uint16_t pagesize = -1);

        void readFromMemory(
            uint16_t MemAddress, 
            uint8_t *pData, 
            uint16_t Size
        ) override;

        void writeToMemory(
            uint16_t MemAddress, 
            uint8_t *pData, 
            uint16_t Size
        ) override;
    
    protected:
        I2C         *_pInstance;
        uint16_t    _DevAddress;
        uint16_t    _pages = 1;
        uint16_t    _pageSize = -1;
};
#endif