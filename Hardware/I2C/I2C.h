#ifndef __I2C_H_
#define __I2C_H_

#include "../../Core.h"
#include "Interface/IBus.h"

#include <queue>

#define I2C_MAX_INSTANCES 2

class I2C : public IBus {
    public:
        I2C(I2C_HandleTypeDef *pHandler);
        static I2C *getInstance(I2C_HandleTypeDef *pHandler);

        void send(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
        void receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size);

        void readFromMemory(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
        void writeToMemory(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
        
        void txInterrupt();
        void rxInterrupt();

    private:
        I2C_HandleTypeDef* _pHandler;

        enum EoperationType {RECEIVE, SEND, MEM_READ, MEM_WRITE};
        
        struct i2cOperation {
            EoperationType operationType;
            uint16_t DevAddress;
            uint16_t MemAddress;
            uint8_t *pData;
            uint16_t Size;
        };
        
        std::queue<i2cOperation> operations;
        
        enum {
            IDLE,
            WORK,
            WAITING,
            FINISH,
        } i2cState = IDLE;

        i2cOperation currentOperation;
};
#endif