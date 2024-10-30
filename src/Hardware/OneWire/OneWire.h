#include "../Timer/Timer.h"
#ifdef __TIMER_H_

#ifndef __ONEWIRE_H_
#define __ONEWIRE_H_

#include "../../Interface/IBus.h"

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
            bool adres = true,//z adresacją ROM
            bool reset = false,//wymuszenie resetu
            dataCallback_f callbackFn = nullptr
        );





        uint16_t queueSize();

    private:
        Timer* timer;
        GPIO_TypeDef* GPIOx;
        uint16_t GPIO_Pin;
/*
#define OW_PROGRESS_END						0

#define OW_PROGRESS_RESET					10//>>>11 --->
#define OW_PROGRESS_RESET_WAIT_TIMER_END	11//>>>0

#define OW_PROGRESS_WRITE					20//>>>21
#define OW_PROGRESS_WRITE_NEXT_BIT			21//>>>22 or 0 --->
#define OW_PROGRESS_WRITE_WAIT_TIMER_END	22//>>>22

#define OW_PROGRESS_READ					30//>>>31
#define OW_PROGRESS_READ_NEXT_BIT			31//>>>32 or 0 --->
#define OW_PROGRESS_READ_WAIT_TIMER_END		32//>>22
*/
//progress irq timer

#define OW_TIMER_PROGRESS_END						0
#define OW_TIMER_PROGRESS_RESET						10
#define OW_TIMER_PROGRESS_RESET_WAIT_END_LOW		11
#define OW_TIMER_PROGRESS_RESET_WAIT_END_HIGH		12
#define OW_TIMER_PROGRESS_RESET_WAIT_READ_STATUS	13

#define OW_TIMER_PROCESS_WRITE						20
#define OW_TIMER_PROCESS_WRITE_WAIT_END_HIGH		21
#define OW_TIMER_PROCESS_WRITE_WAIT_END_LOW			22
#define OW_TIMER_PROCESS_WRITE_WAIT_STATUS			23

#define OW_TIMER_PROGRESS_READ						30
#define OW_TIMER_PROGRESS_READ_WAIT					31
#define OW_TIMER_PROGRESS_READ_GET					32
#define OW_TIMER_PROGRESS_READ_END					33

#define OW_TIM_ELAPSED	ow_tim_delay==0

        uint8_t  ow_tim_bit_index;
        uint8_t  ow_tim_bit;
        uint8_t  ow_tim_delay;
        uint8_t  ow_tim_progress;
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

#endif
#endif