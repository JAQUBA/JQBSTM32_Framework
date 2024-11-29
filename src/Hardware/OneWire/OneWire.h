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
            bool res = false,
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
        Timer* OW_Timer;
        GPIO_TypeDef* OW_Port;
        uint16_t OW_Pin; 

        uint32_t operationTimeout;


        uint8_t  ow_tim_bit_index;

        uint8_t  ow_tim_bit;

        uint8_t  ow_byte_index;
        uint8_t  ow_byte;
        
        bool     is_device_presence;

        enum {
            OW_TIMER_PROGRESS_IDLE,

            OW_TIMER_PROGRESS_RESET,
            OW_TIMER_PROGRESS_RESET_WAIT_END_LOW,
            OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH,

            OW_TIMER_PROGRESS_WRITE,
            OW_TIMER_PROGRESS_WRITE_WAIT_END_HIGH,
            OW_TIMER_PROGRESS_WRITE_WAIT_END_LOW,

            OW_TIMER_PROGRESS_READ,
            OW_TIMER_PROGRESS_READ_WAIT,
            OW_TIMER_PROGRESS_READ_GET
        } ow_tim_progress = OW_TIMER_PROGRESS_IDLE;   

        enum {
            OW_PROGRESS_IDLE,

            OW_PROGRESS_WRITE,
            OW_PROGRESS_WRITE_NEXT_BIT,
            OW_PROGRESS_WRITE_WAIT_TIMER_END,
            OW_PROGRESS_READ,
            OW_PROGRESS_READ_NEXT_BIT,
            OW_PROGRESS_READ_WAIT_TIMER_END
        } ow_progress = OW_PROGRESS_IDLE;

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