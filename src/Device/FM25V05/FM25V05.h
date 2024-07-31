#ifndef _FM25V05_H_
#define _FM25V05_H_

#include "Core.h"
#include "../../Interface/IExternalMemory.h"
#include "../../Hardware/SPI/SPI.h"

class FM25V05 : public IExternalMemory {
    public:
        FM25V05(SPI *pInstance,
            GPIO_TypeDef* GPIOx,
            uint16_t GPIO_Pin,
            uint32_t BaseAddress
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
        SPI             *_pInstance;
        uint32_t        _BaseAddress;

        GPIO_TypeDef    *_CSPort;
        uint16_t        _CSPin;
    
};
#endif