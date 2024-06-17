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

        void send(uint8_t *data, uint16_t length);
        void send(const char *data, uint16_t length);
        
        void onReceiveHandler(void(*onReceive)(uint8_t* data, uint16_t length));
        void onTransmitHandler(void(*onTransmit)());
    private:
        UART_HandleTypeDef *_pHandler;

        void(*fpOnTransmit)();
        void(*fpOnReceive)(uint8_t* data, uint16_t length);

        uint8_t Received_u1;
        
        bool received = false;
        unsigned long lastReceivedByte = 0;

        uint8_t  rx_data_index = 0;
        uint8_t  rx_buffer[256];
};

#endif