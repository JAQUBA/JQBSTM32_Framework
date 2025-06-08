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
 * @class FDCAN
 * @brief Class for handling FDCAN operations.
 */
class FDCAN : public IBus {
    public:
	/**
	 * @brief Gets the instance of the FDCAN.
	 * @param pHandler Pointer to FDCAN_HandleTypeDef structure.
	 * @return Pointer to FDCAN instance.
	 */
	static FDCAN *getInstance(FDCAN_HandleTypeDef *pHandler);

	/**
	 * @brief Constructor for FDCAN class.
	 * @param pHandler Pointer to FDCAN_HandleTypeDef structure.
	 */
	FDCAN(FDCAN_HandleTypeDef *pHandler);

	/**
	 * @brief Registers a callback for a specific communication number.
	 * @param commNumber Communication number.
	 * @param cHandler Callback function to handle the data.
	 */
	void onPacket(uint16_t commNumber, dataCallback_f cHandler);

	/**
	 * @brief Sends data over FDCAN.
	 * @param identifier Message identifier.
	 * @param pData Pointer to data to be sent.
	 * @param Size Size of the data.
	 * @param DataLength Length of the data (default is FDCAN_DLC_BYTES_8).
	 */
	void send(uint32_t identifier, uint8_t *pData, uint16_t Size, uint32_t DataLength = FDCAN_DLC_BYTES_8);

	/**
	 * @brief Handles the receive interrupt.
	 */
	void rxInterrupt();

    private:
		FDCAN_HandleTypeDef *_pInstance; /**< Pointer to FDCAN_HandleTypeDef structure. */
		FDCAN_FilterTypeDef sFilterConfig; /**< Filter configuration structure. */
		
		FDCAN_RxHeaderTypeDef pRxHeader; /**< Receive header structure. */
		FDCAN_TxHeaderTypeDef pTxHeader; /**< Transmit header structure. */

		uint8_t pData[8] = {0,0,0,0,0,0,0,0}; /**< Data buffer. */
		bool hasPacket = false; /**< Flag indicating if a packet has been received. */

		/**
		 * @struct handlerStruct
		 * @brief Structure to hold handler information.
		 */
		struct handler {
			uint32_t commNumber;
			dataCallback_f handler;
		};
		std::list<struct handler> handlers;
};
#endif
#endif
