#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "Core.h"

#include "Interface/IBus.h"
#include "Hardware/Timer/Timer.h"

#define OW_GET_PIN HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)
#define OW_L HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET)
#define OW_H HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET)

class OneWire : public IBus {
    public:
        OneWire(Timer* timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

        void reset();
        void transmit(
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        void receive(
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        void transmitThenReceive(
            uint8_t *pData_tx, uint16_t txSize,
            uint8_t *pData_rx, uint16_t rxSize,
            dataCallback_f callbackFn = nullptr
        );
        uint16_t queueSize();

    private:
        Timer* timer;
        GPIO_TypeDef* GPIOx;
        uint16_t GPIO_Pin;

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
            RESET,
            RECEIVE,
            TRANSMIT
        };
        
        struct operation {
            EoperationType  operationType;
            uint8_t         *pData;
            uint16_t        Size;
            bool            free = false;
            
            dataCallback_f  callback_f = nullptr;
        } currentOperation;
        
        std::queue<operation> operations;

        enum workState {
            OW_LOW,
            OW_HIGH
        };
        struct work {
            workState state = OW_HIGH;
            uint8_t bitIndex = 0;
            uint8_t byteIndex = 0;
            uint8_t *pData;
            uint16_t Size;
            dataCallback_f callback_f;
        } currentWork;

        void work();
};

#endif // ONEWIRE_H