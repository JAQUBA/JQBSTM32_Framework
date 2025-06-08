/*
 * JQBSTM32 Framework - CAN.h Header
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
#ifdef HAL_CAN_MODULE_ENABLED

#ifndef __CAN_H_
#define __CAN_H_

#ifndef CAN_MAX_INSTANCES
#define CAN_MAX_INSTANCES 1  ///< Maximum number of CAN instances supported
#endif

#include "../../Interface/IBus.h"
#include <unordered_map> 

/**
 * @class CAN
 * @brief CAN (Controller Area Network) communication class implementing IBus interface
 * @details This class provides methods for CAN bus communication including message transmission,
 *          reception with callback handling, and interrupt management. Supports standard CAN protocol
 *          with configurable message identifiers and data payloads up to 8 bytes.
 * @note Inherits from IBus interface for standardized bus communication
 */
class CAN : public IBus{
	public:
		/**
		 * @brief Get singleton instance of CAN for specified hardware handle
		 * @param pHandler Pointer to STM32 HAL CAN handle structure
		 * @return Pointer to CAN instance associated with the hardware handle
		 * @details Creates new instance if not exists, otherwise returns existing instance
		 * @note Uses singleton pattern to ensure one instance per hardware peripheral
		 */
		static CAN *getInstance(CAN_HandleTypeDef *pHandler);
		
		/**
		 * @brief Constructor for CAN class
		 * @param pHandler Pointer to STM32 HAL CAN handle structure
		 * @details Initializes CAN instance with hardware handle and sets up default configuration
		 */
		CAN(CAN_HandleTypeDef *pHandler);
		
		/**
		 * @brief Register callback function for specific CAN message identifier
		 * @param commNumber CAN message identifier to listen for
		 * @param cHandler Callback function to execute when message with matching ID is received
		 * @details Allows multiple callbacks for different message identifiers.
		 *          Callback receives data pointer and length as parameters.
		 * @note Callback function signature: void callback(uint8_t* data, uint32_t length)
		 */
		void onPacket(uint16_t commNumber, dataCallback_f cHandler);
		
		/**
		 * @brief Send CAN message with specified parameters
		 * @param identifier CAN message identifier (11-bit or 29-bit depending on configuration)
		 * @param pData Pointer to data buffer to transmit
		 * @param DataLength Number of data bytes to send (0-8)
		 * @param rtr_mode Remote transmission request mode (default: CAN_RTR_DATA for data frame)
		 * @details Transmits data frame over CAN bus. Supports both standard and extended identifiers.
		 * @note Maximum data length is 8 bytes for standard CAN protocol
		 */
		void send(uint32_t identifier, uint8_t *pData,
		 		uint32_t DataLength = 8U, uint32_t rtr_mode = CAN_RTR_DATA);
		
		/**
		 * @brief Handle CAN transmission complete interrupt
		 * @details Called automatically by HAL when transmission is completed.
		 *          Updates internal state and prepares for next transmission.
		 * @note Should be called from CAN TX interrupt handler
		 */
        void txInterrupt();
        
        /**
		 * @brief Handle CAN message reception interrupt
		 * @details Called automatically by HAL when new message is received.
		 *          Processes received data and triggers appropriate callbacks.
		 * @note Should be called from CAN RX interrupt handler
		 */
        void rxInterrupt();
        
        /**
		 * @brief Handle CAN error interrupt
		 * @details Called automatically by HAL when CAN error occurs.
		 *          Handles error conditions and recovery procedures.
		 * @note Should be called from CAN error interrupt handler
		 */
        void errorInterrupt();

	private:
		CAN_HandleTypeDef *_pInstance;    ///< Pointer to STM32 HAL CAN handle
		CAN_FilterTypeDef sFilterConfig;  ///< CAN message filter configuration
		
		CAN_RxHeaderTypeDef pRxHeader;    ///< Reception header structure for incoming messages
		CAN_TxHeaderTypeDef pTxHeader;    ///< Transmission header structure for outgoing messages

		uint32_t canMailbox;              ///< CAN mailbox identifier for transmission
		uint8_t pData[8] = {0,0,0,0,0,0,0,0}; ///< Internal data buffer for message processing
		bool hasPacket = false;           ///< Flag indicating if new packet has been received

		/**
		 * @struct handler
		 * @brief Structure to store callback handler information
		 * @details Associates message identifiers with their corresponding callback functions
		 */
		struct handler {
			uint32_t commNumber;    ///< CAN message identifier to match
			dataCallback_f handler; ///< Callback function to execute on match
		};
		std::list<struct handler> handlers; ///< List of registered message handlers
};

#endif
#endif
