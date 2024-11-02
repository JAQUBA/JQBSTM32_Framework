#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "Core.h"
#include "main.h"

#include "Interface/IBus.h"
#include "Hardware/Timer/Timer.h"

#include "time.h"

class OneWire : public IBus {
    public:
        OneWire(Timer* timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
      
        void TimInterrupt();

        static OneWire *getInstance(TIM_HandleTypeDef *pHandler);

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
            bool reset = false,//wymuszenie resetu
            dataCallback_f callbackFn = nullptr
        );

        void sesja(
            uint8_t ROMcomm,
            uint8_t *adres,
            uint8_t FUNcomm,
            uint8_t *buffer,
            uint8_t size = 0,
            bool    reset = false,
            //bool    read = false,
            dataCallback_f callbackFn = nullptr
        );

        uint16_t queueSize();

    private:
        Timer* timer;
        GPIO_TypeDef* GPIOx;
        uint16_t GPIO_Pin;

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
    } ow_tim_progress = OW_TIMER_PROGRESS_END;

    #define OW_TIM_ELAPSED	ow_tim_delay==0

        uint8_t  ow_tim_bit_index;
        uint8_t  ow_tim_bit;
        uint8_t  ow_tim_delay;
        uint8_t  ow_tim_ready;

        uint8_t  ow_progress;
        uint8_t  ow_byte_size;
        uint8_t  ow_byte_index;
        uint8_t  ow_byte;

        uint32_t operationTimeout;

    enum {
        IDLE,
        CHECK_FREE,
        WORK,
        WAITING,
        CLEAR,
        FINISH,
        OW_PROGRESS_END,
        OW_PROGRESS_RESET,				    //>>>11 --->
        OW_PROGRESS_RESET_WAIT_TIMER_END,	//>>>0

        OW_PROGRESS_WRITE,					//>>>21
        OW_PROGRESS_WRITE_NEXT_BIT,			//>>>22 or 0 --->
        OW_PROGRESS_WRITE_WAIT_TIMER_END,	//>>>22

        OW_PROGRESS_READ,					//>>>31
        OW_PROGRESS_READ_NEXT_BIT,			//>>>32 or 0 --->
        OW_PROGRESS_READ_WAIT_TIMER_END	    //>>22
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
        uint8_t  byte;
        dataCallback_f callback_f;
    } currentWork;

    void work();
};

#endif // ONEWIRE_H