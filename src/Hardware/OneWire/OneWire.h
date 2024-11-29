#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "../../Core.h"
#include "Interface/IBus.h"
#include "Hardware/Timer/Timer.h"

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
            dataCallback_f callbackFn = nullptr,
            bool res = false
        );

        void transaction(
            uint8_t ROMcomm,
            uint8_t *adres,
            uint8_t FUNcomm,
            uint8_t *pData_tx = NULL,
            uint8_t txSize = 0,
            uint8_t *pData_rx = NULL,
            uint8_t rxSize  = 0,
            dataCallback_f callbackFn = nullptr,
            bool    res = false
        );
        uint16_t queueSize();

    private:
        Timer* OW_Timer;
        GPIO_TypeDef* OW_Port;
        uint16_t OW_Pin; 

        uint32_t operationTimeout;
        bool     is_device_presence;

        uint8_t  ow_byte;
        uint8_t  ow_byte_index;
        uint8_t  ow_bit_index;
        
        enum {
            OPERATION_PROGRESS_IDLE,

            OPERATION_PROGRESS_RESET,
            OPERATION_PROGRESS_RESET_WAIT_LOW,
            OPERATION_PROGRESS_RESET_WAIT_HIGH,

            OPERATION_PROGRESS_WRITE_START,
            OPERATION_PROGRESS_WRITE_BIT,
            OPERATION_PROGRESS_WRITE_END,

            OPERATION_PROGRESS_READ_START,
            OPERATION_PROGRESS_READ_BIT,
            OPERATION_PROGRESS_READ_WAIT,
            OPERATION_PROGRESS_READ_END
        } operationProgress = OPERATION_PROGRESS_IDLE;   

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