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
#define CAN_MAX_INSTANCES 1
#endif

#include "../../Interface/IBus.h"
#include <unordered_map> 

class CAN : public IBus{
	public:
		static CAN *getInstance(CAN_HandleTypeDef *pHandler);
		CAN(CAN_HandleTypeDef *pHandler);
		
		void onPacket(uint16_t commNumber, dataCallback_f cHandler);
		void send(uint32_t identifier, uint8_t *pData,
		 		uint32_t DataLength = 8U, uint32_t rtr_mode = CAN_RTR_DATA);
        void txInterrupt();
        void rxInterrupt();
        void errorInterrupt();

	private:
		CAN_HandleTypeDef *_pInstance;
		CAN_FilterTypeDef sFilterConfig;
		
		CAN_RxHeaderTypeDef pRxHeader;
		CAN_TxHeaderTypeDef pTxHeader;

		uint32_t canMailbox;
		uint8_t pData[8] = {0,0,0,0,0,0,0,0};
		bool hasPacket = false;

		struct handler {
			uint32_t commNumber;
			dataCallback_f handler;
		};
		std::list<struct handler> handlers;
};

#endif
#endif
