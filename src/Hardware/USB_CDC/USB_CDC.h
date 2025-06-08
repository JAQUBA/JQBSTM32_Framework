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

/**
 * @brief USB CDC (Communication Device Class) implementation
 * @details Provides USB virtual serial port communication functionality
 * @note Inherits from IBus interface for standardized bus operations
 */
class USB_CDC : public IBus {
    public:
        /**
         * @brief USB_CDC constructor
         * @details Initializes USB CDC communication interface
         */
        USB_CDC();
        
        /**
         * @brief Get USB_CDC instance
         * @details Returns singleton instance of USB_CDC class
         * @return USB_CDC* Pointer to USB_CDC instance
         */
        static USB_CDC *getInstance();

        /**
         * @brief Send data via USB CDC
         * @details Transmits data through USB virtual serial port
         * @param pData Pointer to data buffer to send
         * @param Size Number of bytes to send
         * @param callbackFn Optional callback function called after transmission (default: nullptr)
         */
        void send(uint8_t *pData, uint16_t Size, dataCallback_f callbackFn = nullptr);
        
        /**
         * @brief Send string via USB CDC
         * @details Transmits null-terminated string through USB virtual serial port
         * @param buf Pointer to null-terminated string buffer
         */
        void send(const char *buf);

        /**
         * @brief Set receive callback handler
         * @details Registers callback function to be called when data is received
         * @param onReceive Callback function for received data
         */
        void onReceiveHandler(dataCallback_f onReceive);
        
        /**
         * @brief Set transmit callback handler
         * @details Registers callback function to be called when transmission is complete
         * @param onTransmit Callback function for transmission completion
         */
        void onTransmitHandler(voidCallback_f onTransmit);

        /**
         * @brief USB CDC receive interrupt handler
         * @details Internal function called by USB stack when data is received
         * @param Buf Pointer to received data buffer
         * @param Len Pointer to length of received data
         */
        void rxInterrupt(uint8_t *Buf, uint32_t *Len);
        
        /**
         * @brief USB CDC transmit interrupt handler
         * @details Internal function called by USB stack when transmission is complete
         * @param Buf Pointer to transmitted data buffer
         * @param Len Pointer to length of transmitted data
         * @param epnum Endpoint number
         */
        void txInterrupt(uint8_t *Buf, uint32_t *Len, uint8_t epnum);

        /**
         * @brief Get operation queue size
         * @details Returns the number of pending operations in the queue
         * @return uint16_t Number of pending operations
         */
        uint16_t queueSize();

    private:
        dataCallback_f fpOnReceive; ///< Receive callback function pointer
        voidCallback_f fpOnTransmit; ///< Transmit callback function pointer

        uint32_t operationTimeout; ///< Operation timeout value
        
        /**
         * @brief Operation state enumeration
         * @details Internal states for USB CDC operation state machine
         */
        enum { IDLE, CHECK_FREE, WORK, WAITING, CLEAR, FINISH } operationState = IDLE;
        
        /**
         * @brief Operation type enumeration
         * @details Types of USB CDC operations
         */
        enum EoperationType {RECEIVE, SEND};

        /**
         * @brief Operation structure
         * @details Structure holding information about pending USB CDC operation
         */
        struct operation {
            EoperationType operationType; ///< Type of operation
            uint8_t *pData;              ///< Pointer to data buffer
            uint16_t Size;               ///< Size of data
            dataCallback_f callback_f;   ///< Callback function
            bool free = true;            ///< Operation slot availability flag
        } currentOperation;
        
        std::queue<operation> operations; ///< Queue of pending operations
};
#endif
#endif
