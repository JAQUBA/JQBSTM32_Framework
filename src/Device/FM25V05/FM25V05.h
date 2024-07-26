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
        uint16_t        _MemAddSize = ADDRESS_SIZE_16BIT;

        GPIO_TypeDef    *_CSPort;
        uint16_t        _CSPin;
    
};
#endif

// #ifndef __PCF8583_H_
// #define __PCF8583_H_
// #include "../../Core.h"
// #include "../../Hardware/I2C/I2C.h"
// #include "../../Hardware/SPI/SPI.h"
// #include "../../Interface/IExternalMemory.h"

// class PCF8583 : public IExternalMemory {
//     public:
//         PCF8583(I2C *pInstance,
//             uint16_t DevAddress,
//             uint32_t BaseAddress,
//             uint16_t MemAddSize
//         );
//         void readFromMemory(
//             uint32_t MemAddress,
//             uint8_t *pData, 
//             uint16_t Size
//         ) override;

//         void writeToMemory(
//             uint32_t MemAddress,
//             uint8_t *pData, 
//             uint16_t Size
//         ) override;
        
//     protected:
//         I2C             *_pInstance;
//         uint16_t        _DevAddress;
//         uint32_t        _BaseAddress;
//         uint16_t        _MemAddSize;

//         bool            isSPI = false;

//         SPI             *_pInstanceSPI;
//         GPIO_TypeDef    * _CSPort;
//         uint16_t        _CSPin;
// };
// #endif