/*
 * JQBSTM32 Framework - USB_CDC.h Header
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
#ifdef HAL_PCD_MODULE_ENABLED

#ifndef __USB_CDC_H_
#define __USB_CDC_H_

#include "../../Interface/IBus.h"

class USB_CDC : public IBus {
    public:
        USB_CDC();
        static USB_CDC *getInstance();

        void send(uint8_t *pData, uint16_t Size, dataCallback_f callbackFn = nullptr);
        void send(const char *buf);

        void onReceiveHandler(dataCallback_f onReceive);
        void onTransmitHandler(voidCallback_f onTransmit);

        void rxInterrupt(uint8_t *Buf, uint32_t *Len);
        void txInterrupt(uint8_t *Buf, uint32_t *Len, uint8_t epnum);

        uint16_t queueSize();

    private:
        dataCallback_f fpOnReceive;
        voidCallback_f fpOnTransmit;

        uint32_t operationTimeout;
        enum { IDLE, CHECK_FREE, WORK, WAITING, CLEAR, FINISH } operationState = IDLE;
        enum EoperationType {RECEIVE, SEND};

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
