/*
 * JQBSTM32 Framework - I2C.h Header
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
#ifdef HAL_I2C_MODULE_ENABLED

#ifndef __I2C_H_
#define __I2C_H_

#ifndef I2C_MAX_INSTANCES
#define I2C_MAX_INSTANCES 2
#endif

#ifndef I2C_MAX_OPERATIONS
#define I2C_MAX_OPERATIONS 8 ///< Maximum number of queued I2C operations
#endif

#ifndef I2C_MAX_DATA_SIZE
#define I2C_MAX_DATA_SIZE 128 ///< Maximum size of data for single I2C operation
#endif

#include "../../Interface/IBus.h"

class I2C : public IBus {
    public:
        /**
         * @brief Constructs an I2C instance with the specified handler.
         * 
         * @param pHandler Pointer to the I2C handler.
         */
        I2C(I2C_HandleTypeDef *pHandler);

        /**
         * @brief Gets an instance of the I2C class with the specified handler.
         * 
         * @param pHandler Pointer to the I2C handler.
         * @return I2C* Pointer to the I2C instance.
         */
        static I2C *getInstance(I2C_HandleTypeDef *pHandler);        /**
         * @brief Transmits data to the specified device address.
         * 
         * @param DevAddress The address of the device to transmit to.
         * @param pData Pointer to the data to transmit.
         * @param Size The size of the data to transmit.
         * @param callbackFn Callback function to be called after transmission (default is nullptr).
         * @return bool True if operation was queued successfully, false if queue is full or data too large
         */
        bool transmit(
            uint16_t DevAddress,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );

        /**
         * @brief Receives data from the specified device address.
         * 
         * @param DevAddress The address of the device to receive from.
         * @param pData Pointer to the buffer to store received data.
         * @param Size The size of the data to receive.
         * @param callbackFn Callback function to be called after reception (default is nullptr).
         * @return bool True if operation was queued successfully, false if queue is full
         */
        bool receive(
            uint16_t DevAddress,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );

        /**
         * @brief Reads data from the specified memory address of a device.
         * 
         * @param DevAddress The address of the device.
         * @param MemAddress The memory address to read from.
         * @param MemAddSize The size of the memory address.
         * @param pData Pointer to the buffer to store read data.
         * @param Size The size of the data to read.
         * @param callbackFn Callback function to be called after reading (default is nullptr).
         * @return bool True if operation was queued successfully, false if queue is full
         */
        bool readFromMemory(
            uint16_t DevAddress,
            uint16_t MemAddress,
            uint16_t MemAddSize,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );

        /**
         * @brief Writes data to the specified memory address of a device.
         * 
         * @param DevAddress The address of the device.
         * @param MemAddress The memory address to write to.
         * @param MemAddSize The size of the memory address.
         * @param pData Pointer to the data to write.
         * @param Size The size of the data to write.
         * @param callbackFn Callback function to be called after writing (default is nullptr).
         * @return bool True if operation was queued successfully, false if queue is full or data too large
         */
        bool writeToMemory(
            uint16_t DevAddress,
            uint16_t MemAddress,
            uint16_t MemAddSize,
            uint8_t *pData, uint16_t Size,
            dataCallback_f callbackFn = nullptr
        );

        /**
         * @brief Handles the completion of a transmission.
         * 
         * @param hi2c Pointer to the I2C handler.
         */
        void txInterrupt();

        /**
         * @brief Handles the completion of a reception.
         * 
         * @param hi2c Pointer to the I2C handler.
         */
        void rxInterrupt();

        /**
         * @brief Handles errors during I2C operations.
         * 
         * @param hi2c Pointer to the I2C handler.
         */
        void errorInterrupt();        
        uint16_t queueSize();
        
        /**
         * @brief Check if operation queue is full
         * @return bool True if queue is full
         */
        bool isQueueFull();
        
        /**
         * @brief Get maximum queue capacity
         * @return uint16_t Maximum number of operations that can be queued
         */
        uint16_t getMaxQueueSize();
    private:
        I2C_HandleTypeDef* _pHandler;

        uint32_t operationTimeout;

        enum {
            IDLE,
            CHECK_FREE,
            WORK,
            WAITING,
            CLEAR,
            FINISH
        } operationState = IDLE;

        enum EoperationType {
            TRANSMIT,
            RECEIVE,
            
            MEM_READ,
            MEM_WRITE
        };
        
        struct operation {
            EoperationType operationType;
            uint16_t DevAddress;
            uint16_t MemAddress;
            uint16_t MemAddSize;
            uint8_t *pData;
            uint8_t internalData[I2C_MAX_DATA_SIZE]; ///< Internal buffer for data copies
            uint16_t Size;
            dataCallback_f callback_f;
            bool useInternalBuffer; ///< True if using internal buffer, false if using external pointer
            bool active; ///< True if operation slot is active
            
            operation() : operationType(TRANSMIT), DevAddress(0), MemAddress(0), 
                         MemAddSize(0), pData(nullptr), Size(0), callback_f(nullptr),
                         useInternalBuffer(false), active(false) {}
        };

        operation currentOperation;
        operation _operations[I2C_MAX_OPERATIONS]; ///< Circular buffer for operations
        uint8_t _operationHead; ///< Head index for circular buffer
        uint8_t _operationTail; ///< Tail index for circular buffer
        uint8_t _operationCount; ///< Number of operations in queue
        
        /**
         * @brief Get next operation from queue
         * @return bool True if operation was available, false if queue empty
         */
        bool getNextOperation();
        
        /**
         * @brief Add operation to queue
         * @param op Operation to add
         * @return bool True if successfully added, false if queue full
         */
        bool enqueueOperation(const operation& op);
};
#endif
#endif
