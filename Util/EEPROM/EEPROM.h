#ifndef __EEPROM_H_
#define __EEPROM_H_
#include "../../Core.h"
#include "../../Hardware/I2C/I2C.h"
#include "../../Hardware/SPI/SPI.h"
#include "../../Interface/IExternalMemory.h"

class EEPROM : public IExternalMemory {
    public:
        EEPROM(I2C *pInstance,
            uint16_t DevAddress,
            uint32_t BaseAddress,
            uint16_t MemAddSize);

        EEPROM(SPI *pInstance,
            GPIO_TypeDef* GPIOx,
            uint16_t GPIO_Pin,
            uint32_t BaseAddress);
       
        void readFromMemory(
            uint32_t MemAddress,
            uint16_t MemAddSize, 
            uint8_t *pData, 
            uint16_t Size
        ) override;

        void writeToMemory(
            uint32_t MemAddress,
            uint16_t MemAddSize, 
            uint8_t *pData, 
            uint16_t Size
        ) override;


    
    protected:
        I2C             *_pInstanceI2C;
        uint16_t        _DevAddress;
        uint32_t        _BaseAddress;
        uint16_t        _MemAddSize;

        bool            isSPI = false;

        SPI             *_pInstanceSPI;
        GPIO_TypeDef    * _CSPort;
        uint16_t        _CSPin;
};
#endif