/*
 * JQBSTM32 Framework - UART.h Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "../../Core.h"
#ifdef HAL_UART_MODULE_ENABLED

#ifndef __UART_H_
#define __UART_H_

#ifndef UART_MAX_INSTANCES
#define UART_MAX_INSTANCES 2
#endif

#include "../../Interface/IBus.h"

class UART : public IBus {
    public:
        UART(UART_HandleTypeDef *pHandler, GPIO_TypeDef *dirPort = NULL, uint16_t dirPin = 0);
        static UART *getInstance(UART_HandleTypeDef *pHandler);

        void rxInterrupt();
        void txInterrupt();
        void errorInterrupt();

        void transmit(
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        
        void onReceiveHandler(dataCallback_f onReceive);
        void onTransmitHandler(voidCallback_f onTransmit);
    private:
        UART_HandleTypeDef *_pHandler;

        dataCallback_f fpOnReceive;
        voidCallback_f fpOnTransmit;

        GPIO_TypeDef *_dirPort;
        uint16_t _dirPin;

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
#endif
