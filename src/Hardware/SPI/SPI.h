/*
 * JQBSTM32 Framework - SPI.h Header
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
#ifdef HAL_SPI_MODULE_ENABLED

#ifndef __SPI_H_
#define __SPI_H_

#ifndef SPI_MAX_INSTANCES
#define SPI_MAX_INSTANCES 2
#endif

#include "../../Interface/IBus.h"

/**
 * @brief SPI communication class
 * @details Provides high-speed SPI communication with automatic CS control
 * @note Inherits from IBus interface for standardized bus operations
 */
class SPI : public IBus {
    public:
        /**
         * @brief SPI constructor
         * @details Initializes SPI instance with the specified HAL handler
         * @param pHandler Pointer to HAL SPI handler
         */
        SPI(SPI_HandleTypeDef *pHandler);
        
        /**
         * @brief Get SPI instance
         * @details Returns existing SPI instance or creates new one for the specified handler
         * @param pHandler Pointer to HAL SPI handler
         * @return SPI* Pointer to SPI instance
         */
        static SPI *getInstance(SPI_HandleTypeDef *pHandler);

        /**
         * @brief Transmit data via SPI
         * @details Sends data through SPI with automatic CS control
         * @param CSPort GPIO port for Chip Select pin
         * @param CSPin GPIO pin number for Chip Select
         * @param pData Pointer to data buffer to transmit
         * @param Size Number of bytes to transmit
         * @param callbackFn Optional callback function called after transmission (default: nullptr)
         */
        void transmit(
            GPIO_TypeDef* CSPort, uint16_t CSPin,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        
        /**
         * @brief Receive data via SPI
         * @details Receives data through SPI with automatic CS control
         * @param CSPort GPIO port for Chip Select pin
         * @param CSPin GPIO pin number for Chip Select
         * @param pData Pointer to buffer for received data
         * @param Size Number of bytes to receive
         * @param callbackFn Optional callback function called after reception (default: nullptr)
         */
        void receive(
            GPIO_TypeDef* CSPort, uint16_t CSPin,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );
        
        /**
         * @brief Transmit then receive data via SPI
         * @details First transmits data, then receives data in separate operations
         * @param CSPort GPIO port for Chip Select pin
         * @param CSPin GPIO pin number for Chip Select
         * @param pData_tx Pointer to transmit data buffer
         * @param txSize Number of bytes to transmit
         * @param pData_rx Pointer to receive data buffer
         * @param rxSize Number of bytes to receive
         * @param callbackFn Optional callback function called after operation (default: nullptr)
         */
        void transmitThenReceive(GPIO_TypeDef* CSPort, uint16_t CSPin,
            uint8_t *pData_tx, uint16_t txSize,
            uint8_t *pData_rx, uint16_t rxSize,
            dataCallback_f callbackFn = nullptr
        );
        
        /**
         * @brief Simultaneous transmit and receive via SPI
         * @details Performs full-duplex SPI communication (transmit and receive simultaneously)
         * @param CSPort GPIO port for Chip Select pin
         * @param CSPin GPIO pin number for Chip Select
         * @param pDataTx Pointer to transmit data buffer
         * @param pDataRx Pointer to receive data buffer
         * @param Size Number of bytes to transmit/receive
         * @param callbackFn Optional callback function called after operation (default: nullptr)
         */
        void transmitReceive(
            GPIO_TypeDef* CSPort, uint16_t CSPin,
            uint8_t *pDataTx, uint8_t *pDataRx,
            uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );

        /**
         * @brief SPI transmit interrupt handler
         * @details Internal function called by HAL when transmission is complete
         */
        void txInterrupt();
        
        /**
         * @brief SPI receive interrupt handler
         * @details Internal function called by HAL when reception is complete
         */
        void rxInterrupt();
        
        /**
         * @brief SPI error interrupt handler
         * @details Internal function called by HAL when an error occurs
         */
        void errorInterrupt();

        /**
         * @brief Get operation queue size
         * @details Returns the number of pending operations in the queue
         * @return uint16_t Number of pending operations
         */
        uint16_t queueSize();
    private:
        SPI_HandleTypeDef* _pHandler; ///< HAL SPI handler pointer
        uint32_t operationTimeout;    ///< Operation timeout value
        
        /**
         * @brief Operation state enumeration
         * @details Internal states for SPI operation state machine
         */
        enum {
            IDLE,      ///< No operation in progress
            CHECK_FREE,///< Checking if SPI bus is free
            WORK,      ///< Operation in progress
            WAITING,   ///< Waiting for operation completion
            CLEAR,     ///< Clearing operation
            FINISH     ///< Operation finished
        } operationState = IDLE;

        /**
         * @brief Operation type enumeration
         * @details Defines types of SPI operations
         */
        enum EoperationType {
            RECEIVE,           ///< Receive operation
            TRANSMIT,          ///< Transmit operation
            TRANSMIT_RECEIVE   ///< Full-duplex operation
        };
        
        /**
         * @brief Operation structure
         * @details Structure holding information about pending SPI operation
         */
        struct operation {
            EoperationType  operationType; ///< Type of operation
            
            GPIO_TypeDef*   _CSPort;      ///< Chip Select GPIO port
            uint16_t        _CSPin;       ///< Chip Select GPIO pin
            bool            _pinReset = true; ///< CS pin reset flag
            uint8_t         *pData;       ///< Pointer to data buffer
            uint16_t        Size;         ///< Size of data
            bool            free = false; ///< Operation slot availability flag
            
            dataCallback_f  callback_f = nullptr; ///< Callback function
        } currentOperation;
        
        std::queue<operation> operations; ///< Queue of pending operations
};
#endif
#endif
