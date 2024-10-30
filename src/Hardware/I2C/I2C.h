#ifndef __I2C_H_
#define __I2C_H_

#include "../../Core.h"
#ifdef HAL_I2C_MODULE_ENABLED
#include "Interface/IBus.h"

#ifndef I2C_MAX_INSTANCES
#define I2C_MAX_INSTANCES 2
#endif

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
        static I2C *getInstance(I2C_HandleTypeDef *pHandler);

        /**
         * @brief Transmits data to the specified device address.
         * 
         * @param DevAddress The address of the device to transmit to.
         * @param pData Pointer to the data to transmit.
         * @param Size The size of the data to transmit.
         * @param callbackFn Callback function to be called after transmission (default is nullptr).
         */
        void transmit(
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
         */
        void receive(
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
         */
        void readFromMemory(
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
         */
        void writeToMemory(
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
            uint16_t Size;
            dataCallback_f callback_f;
            bool free = true;
        } currentOperation;

        std::queue<operation> operations;
};
#endif
#endif