#ifndef __NV_RAM_H_
#define __NV_RAM_H_
#include "../../Core.h"
#include "../../Hardware/I2C/I2C.h"
#include "../../Hardware/SPI/SPI.h"
#include "../../Interface/IExternalMemory.h"

class NV_RAM : public IExternalMemory {
    public:
        NV_RAM(I2C *pInstance, uint16_t DevAddress);
        NV_RAM(SPI *pInstance, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

        void readFromMemory(
            uint16_t MemAddress, 
            uint8_t *pData, 
            uint16_t Size, 
            std::function<void(memOperation *operation)> functionPointer = nullptr
        ) override;

        void writeToMemory(
            uint16_t MemAddress, 
            uint8_t *pData, 
            uint16_t Size, 
            std::function<void(memOperation *operation)> functionPointer = nullptr
        ) override;

    protected:
        I2C *_pInstance;
        SPI *_pInstance2;
        uint16_t    _DevAddress;

        
};
#endif