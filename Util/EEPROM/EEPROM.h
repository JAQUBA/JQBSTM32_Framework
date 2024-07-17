#ifndef __EEPROM_H_
#define __EEPROM_H_
#include "../../Core.h"
#include "../../Hardware/I2C/I2C.h"
#include "../../Hardware/SPI/SPI.h"
#include "../../Interface/IExternalMemory.h"

class EEPROM : public IExternalMemory {
    public:
        EEPROM(I2C *pInstance, uint16_t DevAddress, uint16_t MemAdress, uint16_t size);
        EEPROM(SPI *pInstance, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint16_t MemAdress, uint16_t size);
        EEPROM(SPI *pInstance, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t MemAdress, uint16_t size);

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

        bool            isSPI = false;

        SPI             *_pInstanceSPI;
        GPIO_TypeDef    * _CSPort;
        uint16_t        _CSPin;
        
};
#endif