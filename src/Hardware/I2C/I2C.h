#ifndef __I2C_H_
#define __I2C_H_

#include "../../Core.h"
#include "Interface/IBus.h"

#define I2C_MAX_INSTANCES 2

class I2C : public IBus {
    public:
        I2C(I2C_HandleTypeDef *pHandler);
        static I2C *getInstance(I2C_HandleTypeDef *pHandler);

        void transmit(
            uint16_t DevAddress,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );

        void receive(
            uint16_t DevAddress,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );

        void readFromMemory(
            uint16_t DevAddress,
            uint16_t MemAddress,
            uint16_t MemAddSize,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );

        void writeToMemory(
            uint16_t DevAddress,
            uint16_t MemAddress,
            uint16_t MemAddSize,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        
        void txInterrupt();
        void rxInterrupt();
        void errorInterrupt();
        uint16_t queueSize();
    private:
        I2C_HandleTypeDef* _pHandler;

        uint32_t operationTimeout;

        enum {
            IDLE,
            CHECK_FREE,
            WORK,
            WAITING,
            CLEAR,
            FINISH
        } operationState = IDLE;

        enum EoperationType {
            TRANSMIT,
            RECEIVE,
            
            MEM_READ,
            MEM_WRITE
        };
        
        struct operation {
            EoperationType operationType;
            uint16_t DevAddress;
            uint16_t MemAddress;
            uint16_t MemAddSize;
            uint8_t *pData;
            uint16_t Size;
            dataCallback_f callback_f;
            bool free = true;
        } currentOperation;

        std::queue<operation> operations;
};
#endif