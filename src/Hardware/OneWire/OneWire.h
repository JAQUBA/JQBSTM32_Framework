#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "../../Core.h"
#include "Interface/IBus.h"
#include "Hardware/Timer/Timer.h"

class OneWire : public IBus {
    public:
        OneWire(Timer* timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
        void TimInterrupt();
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
            bool res = false,//wymuszenie resetu
            dataCallback_f callbackFn = nullptr
        );

        void sesja(
            uint8_t ROMcomm,
            uint8_t *adres,
            uint8_t FUNcomm,
            uint8_t *pData_tx = NULL,
            uint8_t txSize = 0,
            uint8_t *pData_rx = NULL,
            uint8_t rxSize  = 0,
            bool    res = false,
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
};

#endif // ONEWIRE_H