#ifndef __SPI_H
#define __SPI_H

#include "../../Core.h"
#include "Interface/IBus.h"

#define SPI_MAX_INSTANCES 2



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
        void readFromMemory(uint32_t MemAddress,
            uint16_t MemAddSize,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
            );
        void writeToMemory(uint32_t MemAddress,
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
        uint8_t buff_add[4]={0,0,0,0};

        uint32_t operationTimeout;
        enum {IDLE, CHECK_FREE, WORK, WAITING,
         WAIT_CMD_WREN_END, WRITE, WAIT_WRITE_END, WAIT_CMD_WRDI_END,CMD_WRDI,WAIT_READ_END,READ_END,
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