#include "../../Hardware/SPI/SPI.h"
#ifdef __SPI_H_

#ifndef __FM25V05_H_
#define __FM25V05_H_

#include "../../Interface/IExternalMemory.h"

class FM25V05 : public IExternalMemory {
    public:
        FM25V05(SPI *pInstance,
            GPIO_TypeDef* GPIOx,
            uint16_t GPIO_Pin
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

        GPIO_TypeDef    *_CSPort;
        uint16_t        _CSPin;
    
};
#endif
#endif