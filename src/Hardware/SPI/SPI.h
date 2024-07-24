#ifndef __SPI_H
#define __SPI_H

#include "../../Core.h"
#include "Interface/IBus.h"

#define SPI_MAX_INSTANCES 2
#define SPI_MEMADD_SIZE_1_BYTE 1
#define SPI_MEMADD_SIZE_2_BYTE 2
#define SPI_MEMADD_SIZE_3_BYTE 3

extern uint32_t err;//testy!

class SPI : public IBus {
    public:
        SPI(SPI_HandleTypeDef *pHandler);
        static SPI *getInstance(SPI_HandleTypeDef *pHandler);

        void transmit(GPIO_TypeDef* GPIOx,
            uint16_t GPIO_Pin,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
            );
        void receive(GPIO_TypeDef* GPIOx,
            uint16_t GPIO_Pin,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
            );
        void readFromMemory(GPIO_TypeDef* GPIOx,
            uint16_t GPIO_Pin,
            uint32_t MemAddress,
            uint16_t MemAddSize,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
            );
        void writeToMemory(GPIO_TypeDef* GPIOx,
            uint16_t GPIO_Pin,
            uint32_t MemAddress,
            uint16_t MemAddSize,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
            );
        
        void txInterrupt();
        void rxInterrupt();
        void errorInterrupt();
        uint16_t queueSize();
    private:
        SPI_HandleTypeDef* _pHandler;
        uint32_t operationTimeout;
        enum {IDLE, CHECK_FREE, WORK, WAITING,
              CLEAR, FINISH} operationState = IDLE;
        enum EoperationType {RECEIVE, TRANSMIT,
            MEM_READ, MEM_WRITE};
        
        struct operation {
            EoperationType  operationType;
            uint32_t        MemAddress;
            uint16_t        MemAddSize;
            GPIO_TypeDef*   GPIOx;
            uint16_t        GPIO_Pin;
            uint8_t         *pData_tx;
            uint8_t         *pData_rx;
            uint16_t        Size;
            dataCallback_f  callback_f;
            bool free = true;
        } currentOperation;
        std::queue<operation> operations;
};
#endif