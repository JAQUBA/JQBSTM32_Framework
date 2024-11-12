#ifndef __ONEWIRE_H_
#define __ONEWIRE_H_

#include "../../Core.h"
#include "Interface/IBus.h"
#include "Hardware/Timer/Timer.h"

#define OW_TIM_ELAPSED	ow_tim_delay==0

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
        enum {
            OW_PROGRESS_END,
            OW_PROGRESS_RESET,
            OW_PROGRESS_RESET_WAIT_TIMER_END,
            OW_PROGRESS_WRITE,
            OW_PROGRESS_WRITE_NEXT_BIT,
            OW_PROGRESS_WRITE_WAIT_TIMER_END,
            OW_PROGRESS_READ,
            OW_PROGRESS_READ_NEXT_BIT,
            OW_PROGRESS_READ_WAIT_TIMER_END
        };
        enum {
            OW_TIMER_PROGRESS_END,
            OW_TIMER_PROGRESS_RESET,
            OW_TIMER_PROGRESS_RESET_WAIT_END_LOW,
            OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH,
            OW_TIMER_PROGRESS_RESET_WAIT_READ_STATUS,
            OW_TIMER_PROCESS_WRITE,
            OW_TIMER_PROCESS_WRITE_WAIT_END_HIGH,
            OW_TIMER_PROCESS_WRITE_WAIT_END_LOW,
            OW_TIMER_PROCESS_WRITE_WAIT_STATUS,
            OW_TIMER_PROGRESS_READ,
            OW_TIMER_PROGRESS_READ_WAIT,
            OW_TIMER_PROGRESS_READ_GET,
            OW_TIMER_PROGRESS_READ_END
        };

        uint32_t operationTimeout;

        uint8_t  ow_tim_bit_index;
        uint8_t  ow_tim_bit;
        uint8_t  ow_tim_delay;
        uint8_t  ow_tim_ready;
        uint8_t  ow_tim_progress;

        uint8_t  ow_progress;
        uint8_t  ow_byte_index;
        uint8_t  ow_byte;
        bool     ow_presents;

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

#endif