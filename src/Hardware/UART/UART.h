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

/**
 * @brief UART communication class
 * @details Provides asynchronous UART communication with RS232/RS485 support
 * @note Inherits from IBus interface for standardized bus operations
 */
class UART : public IBus {
    public:
        /**
         * @brief UART constructor
         * @details Initializes UART instance with optional direction control for RS485
         * @param pHandler Pointer to HAL UART handler
         * @param dirPort GPIO port for direction control (NULL for RS232, default: NULL)
         * @param dirPin GPIO pin for direction control (0 for RS232, default: 0)
         */
        UART(UART_HandleTypeDef *pHandler, GPIO_TypeDef *dirPort = NULL, uint16_t dirPin = 0);
        
        /**
         * @brief Get UART instance
         * @details Returns existing UART instance or creates new one for the specified handler
         * @param pHandler Pointer to HAL UART handler
         * @return UART* Pointer to UART instance
         */
        static UART *getInstance(UART_HandleTypeDef *pHandler);

        /**
         * @brief UART receive interrupt handler
         * @details Internal function called by HAL when data is received
         */
        void rxInterrupt();
        
        /**
         * @brief UART transmit interrupt handler
         * @details Internal function called by HAL when transmission is complete
         */
        void txInterrupt();
        
        /**
         * @brief UART error interrupt handler
         * @details Internal function called by HAL when an error occurs
         */
        void errorInterrupt();

        /**
         * @brief Transmit data via UART
         * @details Sends data through UART with optional callback on completion
         * @param pData Pointer to data buffer to transmit
         * @param Size Number of bytes to transmit
         * @param callbackFn Optional callback function called after transmission (default: nullptr)
         */
        void transmit(
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        
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
    private:
        UART_HandleTypeDef *_pHandler; ///< HAL UART handler pointer

        dataCallback_f fpOnReceive; ///< Receive callback function pointer
        voidCallback_f fpOnTransmit; ///< Transmit callback function pointer

        GPIO_TypeDef *_dirPort; ///< Direction control GPIO port (for RS485)
        uint16_t _dirPin;       ///< Direction control GPIO pin (for RS485)

        uint8_t Received_u1;    ///< Single received byte buffer
        
        bool received = false;           ///< Reception flag
        unsigned long lastReceivedByte = 0; ///< Timestamp of last received byte
        uint8_t  rx_data_index = 0;     ///< Current index in receive buffer
        uint8_t  rx_buffer[256];        ///< Receive buffer

        uint32_t operationTimeout; ///< Operation timeout value
        
        /**
         * @brief Operation state enumeration
         * @details Internal states for UART operation state machine
         */
        enum {IDLE, CHECK_FREE, WORK, WAITING, CLEAR, FINISH} operationState = IDLE;
        
        /**
         * @brief Operation type enumeration
         * @details Defines types of UART operations
         */
        enum EoperationType {SEND};
        
        /**
         * @brief Operation structure
         * @details Structure holding information about pending UART operation
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
