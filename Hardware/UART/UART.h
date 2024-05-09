#ifndef __UART_H_
#define __UART_H_

#include <usart.h>

#include "../../Core.h"

#define UART_MAX_INSTANCES 2

class UART {
    public:
        UART_HandleTypeDef *_pInstance;
        UART(UART_HandleTypeDef *_instance);

        static UART *getInstance(UART_HandleTypeDef *_instance);

        void rxInterrupt();
        void txInterrupt();

        void init();
        void poll();

        void send(uint8_t *data, uint16_t length);
        void send(const char *data, uint16_t length);
        
        void onReceiveHandler(void(*onReceive)(uint8_t* data, uint16_t length));
        void onTransmitHandler(void(*onTransmit)());
    private:
        void(*fpOnTransmit)();
        void(*fpOnReceive)(uint8_t* data, uint16_t length);

        uint8_t Received_u1;
        
        bool received = false;
        unsigned long lastReceivedByte = 0;

        uint8_t  rx_data_index = 0;
        uint8_t  rx_buffer[256];
};

#endif