/*
 * JQBSTM32 Framework - FDCAN.h Header
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
#ifdef HAL_FDCAN_MODULE_ENABLED

#ifndef __FDCAN_H_
#define __FDCAN_H_

#ifndef FDCAN_MAX_INSTANCES
#define FDCAN_MAX_INSTANCES 1
#endif

#include "../../Interface/IBus.h"
#include <unordered_map> 

/**
 * @file FDCAN.h
 * @brief FDCAN (Flexible Data-Rate CAN) communication interface for STM32
 * @details Provides a high-level interface for FDCAN communication with automatic
 *          message filtering, callback handling, and interrupt-driven reception.
 *          Implements the IBus interface for standardized communication patterns.
 * @author JAQUBA
 * @version 1.0
 * @date 2024
 * 
 * @note Requires HAL_FDCAN_MODULE_ENABLED to be defined
 * @note Maximum number of instances can be configured with FDCAN_MAX_INSTANCES
 * @warning Ensure proper FDCAN peripheral initialization before use
 */

/**
 * @class FDCAN
 * @brief FDCAN communication handler with callback-based message processing
 * @details This class provides a comprehensive interface for FDCAN communication
 *          including automatic filtering, callback registration, and interrupt handling.
 *          Inherits from IBus interface for standardized communication patterns.
 * 
 * Key features:
 * - Singleton pattern with getInstance() method
 * - Callback-based message handling with communication number filtering
 * - Automatic interrupt-driven message reception
 * - Support for standard and extended FDCAN frames
 * - Configurable data length codes (DLC)
 * 
 * @note Uses STM32 HAL FDCAN driver
 * @note Supports both standard (11-bit) and extended (29-bit) identifiers
 * @warning Proper FDCAN peripheral configuration required before instantiation
 */
class FDCAN : public IBus {
    public:
	/**
	 * @brief Gets the singleton instance of the FDCAN class
	 * @param pHandler Pointer to initialized FDCAN_HandleTypeDef structure
	 * @return Pointer to FDCAN instance (creates new if doesn't exist)
	 * @details Implements singleton pattern to ensure only one instance per handler.
	 *          First call creates the instance, subsequent calls return existing instance.
	 * @note Caller responsible for proper HAL initialization of pHandler
	 * @warning pHandler must remain valid for the lifetime of the instance
	 */
	static FDCAN *getInstance(FDCAN_HandleTypeDef *pHandler);

	/**
	 * @brief Constructor for FDCAN class
	 * @param pHandler Pointer to initialized FDCAN_HandleTypeDef structure
	 * @details Initializes FDCAN instance with given handler and sets up
	 *          default filter configuration for message reception.
	 * @note Usually called internally by getInstance(), not directly by user
	 * @warning pHandler must be properly initialized before construction
	 */
	FDCAN(FDCAN_HandleTypeDef *pHandler);

	/**
	 * @brief Registers a callback for specific communication identifier
	 * @param commNumber Communication identifier to filter messages
	 * @param cHandler Callback function to handle received data
	 * @details Associates a callback function with a specific message identifier.
	 *          When a message with matching identifier is received, the callback
	 *          will be invoked with the message data.
	 * @note Multiple callbacks can be registered for different identifiers
	 * @note Callback signature: void callback(uint8_t* data, uint16_t size)
	 */
	void onPacket(uint16_t commNumber, dataCallback_f cHandler);

	/**
	 * @brief Sends data over FDCAN bus
	 * @param identifier CAN message identifier (11-bit standard or 29-bit extended)
	 * @param pData Pointer to data buffer to transmit
	 * @param Size Number of bytes to send (actual data size)
	 * @param DataLength FDCAN Data Length Code (default: FDCAN_DLC_BYTES_8)
	 * @details Transmits data frame with specified identifier and data.
	 *          DataLength parameter specifies the DLC field value, which may
	 *          differ from actual data size for FDCAN optimization.
	 * @note Size should not exceed the capacity indicated by DataLength
	 * @note Common DataLength values: FDCAN_DLC_BYTES_0 to FDCAN_DLC_BYTES_64
	 * @warning Ensure FDCAN peripheral is properly started before sending
	 */
	void send(uint32_t identifier, uint8_t *pData, uint16_t Size, uint32_t DataLength = FDCAN_DLC_BYTES_8);

	/**
	 * @brief Handles FDCAN receive interrupt
	 * @details Called automatically by interrupt handler when new message arrives.
	 *          Processes received message and invokes appropriate callback if registered.
	 *          Extracts message identifier and data, then searches for matching callback.
	 * @note This method should be called from FDCAN RX interrupt handler
	 * @note Automatically manages message reception and callback invocation
	 * @warning Must be called from appropriate interrupt context
	 */
	void rxInterrupt();

    private:
		FDCAN_HandleTypeDef *_pInstance; /**< Pointer to STM32 HAL FDCAN handle structure */
		FDCAN_FilterTypeDef sFilterConfig; /**< FDCAN filter configuration for message acceptance */
		
		FDCAN_RxHeaderTypeDef pRxHeader; /**< Receive message header containing ID and metadata */
		FDCAN_TxHeaderTypeDef pTxHeader; /**< Transmit message header for outgoing messages */

		uint8_t pData[8] = {0,0,0,0,0,0,0,0}; /**< Temporary data buffer for message processing */
		bool hasPacket = false; /**< Flag indicating whether a new packet has been received */

		/**
		 * @struct handler
		 * @brief Structure containing callback handler information
		 * @details Associates communication identifiers with their corresponding
		 *          callback functions for message processing.
		 */
		struct handler {
			uint32_t commNumber; /**< Communication identifier for message filtering */
			dataCallback_f handler; /**< Callback function for message processing */
		};
		
		/**
		 * @brief List of registered message handlers
		 * @details Contains all registered callback handlers with their associated
		 *          communication identifiers for message routing.
		 */
		std::list<struct handler> handlers;
};
#endif
#endif
