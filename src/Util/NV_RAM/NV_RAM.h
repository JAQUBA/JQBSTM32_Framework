#ifndef __NV_RAM_H_
#define __NV_RAM_H_
#include "../../Core.h"
#include "../../Interface/IExternalMemory.h"
#include "../../Interface/IBus.h"

#include "../../Hardware/I2C/I2C.h"
#include "../../Hardware/SPI/SPI.h"


class NV_RAM : public IExternalMemory {
    public:
        NV_RAM(I2C *pInstance, uint16_t DevAddress, uint16_t pages = 1, uint16_t pagesize = -1);
        NV_RAM(SPI *pInstance, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

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
        I2C         *_pInstanceI2C;
        uint16_t    _DevAddress;

        bool isSPI = false;
        SPI         *_pInstanceSPI;
        GPIO_TypeDef* _CSPort;
        uint16_t    _CSPin;
};
#endif