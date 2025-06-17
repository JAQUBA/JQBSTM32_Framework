/*
 * JQBSTM32 Framework - OneWire Protocol Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * Implementation according to Dallas/Maxim 1-Wire specification.
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

#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "../../Core.h"
#include "Interface/IBus.h"
#include "Hardware/Timer/Timer.h"

/**
 * @brief OneWire communication protocol class
 * @details Implementation of Dallas/Maxim 1-Wire communication protocol
 * @note Inherits from IBus interface for standardized bus operations
 */
class OneWire : public IBus {
	public:
		/**
		 * @brief OneWire constructor
		 * @details Initializes OneWire interface with timer and GPIO configuration
		 * @param timer Pointer to Timer instance for precise timing
		 * @param GPIOx GPIO port for OneWire data line
		 * @param GPIO_Pin GPIO pin for OneWire data line
		 */
		OneWire(Timer* timer, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

		/**
		 * @brief Reset OneWire bus
		 * @details Performs bus reset sequence and checks for device presence
		 */
		void reset();

		/**
		 * @brief Transmit data via OneWire
		 * @details Sends data through OneWire bus with optional callback
		 * @param pData Pointer to data buffer to transmit
		 * @param size Number of bytes to transmit
		 * @param callbackFn Optional callback function called after transmission (default: nullptr)
		 */
		void transmit(
			const uint8_t* pData, uint16_t size,
			dataCallback_f callbackFn = nullptr
		);

		/**
		 * @brief Receive data via OneWire
		 * @details Receives data through OneWire bus with optional callback
		 * @param pData Pointer to buffer for received data
		 * @param size Number of bytes to receive
		 * @param callbackFn Optional callback function called after reception (default: nullptr)
		 */
		void receive(
			uint8_t* pData, uint16_t size,
			dataCallback_f callbackFn = nullptr
		);

		/**
		 * @brief Transmit then receive via OneWire
		 * @details First transmits data, then receives data in sequence
		 * @param pData_tx Pointer to transmit data buffer
		 * @param txSize Number of bytes to transmit
		 * @param pData_rx Pointer to receive data buffer
		 * @param rxSize Number of bytes to receive
		 * @param callbackFn Optional callback function called after operation (default: nullptr)
		 */
		void transmitThenReceive(
			const uint8_t* pData_tx, uint16_t txSize,
			uint8_t* pData_rx, uint16_t rxSize,
			dataCallback_f callbackFn = nullptr
		);

		/**
		 * @brief Complete OneWire transaction
		 * @details Performs full OneWire transaction with ROM and function commands
		 * @param romCommand ROM command byte (SKIP_ROM, MATCH_ROM, etc.)
		 * @param address Pointer to device address (8 bytes for MATCH_ROM)
		 * @param functionCommand Function command byte
		 * @param pData_tx Pointer to transmit data buffer (optional)
		 * @param txSize Number of bytes to transmit (default: 0)
		 * @param pData_rx Pointer to receive data buffer (optional)
		 * @param rxSize Number of bytes to receive (default: 0)
		 * @param callbackFn Optional callback function (default: nullptr)
		 * @param resetAfterTransaction Reset bus after transaction (default: false)
		 */
		void transaction(
			uint8_t romCommand,
			const uint8_t* address,
			uint8_t functionCommand,
			const uint8_t* pData_tx = nullptr,
			uint16_t txSize = 0,
			uint8_t* pData_rx = nullptr,
			uint16_t rxSize = 0,
			dataCallback_f callbackFn = nullptr,
			bool resetAfterTransaction = false
		);
				/**
		 * @brief Get operation queue size
		 * @details Returns the number of pending operations in the queue
		 * @return uint16_t Number of pending operations
		 */
		uint16_t queueSize();

		/**
		 * @brief Check if device is present on the bus
		 * @details Returns the result of the last reset pulse
		 * @return bool True if device responded to reset pulse
		 */
		bool isDevicePresent();

		/**
		 * @brief Check if OneWire operation is in progress
		 * @details Returns true if any operation is currently being executed
		 * @return bool True if operation is busy
		 */
		bool isBusy();

		/**
		 * @brief Clear all pending operations
		 * @details Removes all operations from the queue
		 */
		void clearQueue();

		/**
		 * @brief Calculate CRC8 checksum
		 * @details Calculates CRC8 using Dallas/Maxim polynomial (0x31)
		 * @param data Pointer to data buffer
		 * @param length Number of bytes to calculate CRC for
		 * @return uint8_t Calculated CRC8 value
		 */
		static uint8_t calculateCRC8(const uint8_t* data, uint8_t length);

		/**
		 * @brief Validate CRC8 checksum
		 * @details Validates data against expected CRC8 value
		 * @param data Pointer to data buffer
		 * @param length Number of bytes to validate
		 * @param expectedCRC Expected CRC8 value
		 * @return bool True if CRC is valid
		 */
		static bool validateCRC(const uint8_t* data, uint8_t length, uint8_t expectedCRC);

	private:
		Timer* OW_Timer;      ///< Timer instance for precise timing
		GPIO_TypeDef* OW_Port; ///< GPIO port for OneWire data line
		uint16_t OW_Pin;      ///< GPIO pin for OneWire data line

		uint32_t operationTimeout;  ///< Operation timeout value
		bool     is_device_presence; ///< Device presence detection flag

		uint8_t  ow_byte;       ///< Current byte being processed
		uint8_t  ow_byte_index; ///< Current byte index in buffer
		uint8_t  ow_bit_index;  ///< Current bit index in byte
		
		/**
		 * @brief Operation progress states
		 * @details Internal states for OneWire operation progress tracking
		 */
		enum {
			OPERATION_PROGRESS_IDLE,        ///< No operation in progress

			OPERATION_PROGRESS_RESET,       ///< Reset sequence in progress
			OPERATION_PROGRESS_RESET_WAIT_LOW,  ///< Waiting for low during reset
			OPERATION_PROGRESS_RESET_WAIT_HIGH, ///< Waiting for high during reset

			OPERATION_PROGRESS_WRITE_START, ///< Starting write operation
			OPERATION_PROGRESS_WRITE_BIT,   ///< Writing bit
			OPERATION_PROGRESS_WRITE_END,   ///< Ending write operation

			OPERATION_PROGRESS_READ_START,  ///< Starting read operation
			OPERATION_PROGRESS_READ_BIT,    ///< Reading bit
			OPERATION_PROGRESS_READ_WAIT,   ///< Waiting during read
			OPERATION_PROGRESS_READ_END     ///< Ending read operation
		} operationProgress = OPERATION_PROGRESS_IDLE;   

		/**
		 * @brief Operation state enumeration
		 * @details High-level operation states
		 */
		enum {
			IDLE,      ///< No operation in progress
			CHECK_FREE,///< Checking if bus is free
			WORK,      ///< Operation in progress
			WAITING,   ///< Waiting for operation completion
			CLEAR,     ///< Clearing operation
			FINISH     ///< Operation finished
		} operationState = IDLE;

		/**
		 * @brief Operation type enumeration
		 * @details Types of OneWire operations
		 */
		enum EoperationType {
			RESET,   ///< Reset operation
			RECEIVE, ///< Receive operation
			TRANSMIT ///< Transmit operation
		};
			
		/**
		 * @brief Operation structure
		 * @details Structure holding information about pending OneWire operation
		 */
		struct operation {
			EoperationType  operationType; ///< Type of operation
			uint8_t         *pData;       ///< Pointer to data buffer
			uint16_t        Size;         ///< Size of data
			bool            free = false; ///< Operation slot availability flag
			dataCallback_f  callback_f = nullptr; ///< Callback function
		} currentOperation;
		std::queue<operation> operations; ///< Queue of pending operations
};

#endif // ONEWIRE_H