#ifndef __UART_H_
#define __UART_H_

#include "../../Core.h"
#include "Interface/IBus.h"

#define UART_MAX_INSTANCES 2

class UART : public IBus {
    public:
        UART(UART_HandleTypeDef *pHandler);
        static UART *getInstance(UART_HandleTypeDef *pHandler);

        void rxInterrupt();
        void txInterrupt();

        void send(uint8_t *pData, uint16_t Size, dataCallback_f callbackFn = nullptr);
        
        void onReceiveHandler(std::function<void(uint8_t* data, uint16_t length)> onReceive);
        void onTransmitHandler(std::function<void()> onTransmit);
    private:
        UART_HandleTypeDef *_pHandler;

        std::function<void()> fpOnTransmit;
        std::function<void(uint8_t* data, uint16_t length)> fpOnReceive;

        uint8_t Received_u1;
        
        bool received = false;
        unsigned long lastReceivedByte = 0;

        uint8_t  rx_data_index = 0;
        uint8_t  rx_buffer[256];

        uint32_t operationTimeout;
        enum {IDLE, CHECK_FREE, WORK, WAITING, CLEAR, FINISH} operationState = IDLE;
        enum EoperationType {SEND};
        
        struct operation {
            EoperationType operationType;
            uint8_t *pData;
            uint16_t Size;
            dataCallback_f callback_f;
            bool free = true;
        } currentOperation;
        
        std::queue<operation> operations;
};

#endif